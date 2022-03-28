
#include "RoboCatPCH.h"
#include <thread>

#if _WIN32



enum Packettype
{
	PT_StartConnection,
	PT_ReplicationData,
	PT_EndConnection,
	PT_MAX
};

#define CLASS_IDENTIFICATION(inCode, inClass)\
	enum{kClassId = inCode}; \
	virtual uint32_t GetClassId() const {return kClassId;} \
	static GameObject* CreateInstance() {return new inClass();}

class GameObject
{
public:
	//...
	CLASS_IDENTIFICATION('GOBJ', GameObject)
		//...

		void Write(OutputMemoryBitStream& inStream)
	{
		inStream.Write(mPositionX);
		inStream.Write(mPositionY);

	}
	void Read(InputMemoryBitStream& inStream)
	{
		inStream.Read(mPositionX);
		inStream.Read(mPositionY);
	}
private:
	int mPositionX;
	int mPositionY;
};

class Square : public GameObject
{
public:
	//...
	CLASS_IDENTIFICATION('RBCT', Square)
		//...
};
class Triangle : public GameObject
{
public:
	//...
	CLASS_IDENTIFICATION('RBMS', Triangle)
		//...
};

typedef GameObject* (*GameObjectCreationFunc)();

class ObjectCreationRegistry
{
public:
	static ObjectCreationRegistry& Get()
	{
		static ObjectCreationRegistry sInstance;
		return sInstance;
	}
	template<class T >
	void RegisterCreationFunction()
	{
		//ensure no duplicate class id
		assert(mNameToGameObjectCreationFunctionMap.find(T::kClassId) ==
			mNameToGameObjectCreationFunctionMap.end());
		mNameToGameObjectCreationFunctionMap[T::kClassId] =
			T::CreateInstance;
	}

	GameObject* CreateGameObject(uint32_t inClassId)
	{
		//add error checking if desired- for now crash if not found
		GameObjectCreationFunc creationFunc =
			mNameToGameObjectCreationFunctionMap[inClassId];
		GameObject* gameObject = creationFunc();
		return gameObject;
	}

private:
	ObjectCreationRegistry() {}
	unordered_map<uint32_t, GameObjectCreationFunc>
		mNameToGameObjectCreationFunctionMap;
};

void RegisterObjectCreation()
{
	ObjectCreationRegistry::Get().RegisterCreationFunction<GameObject>();
	ObjectCreationRegistry::Get().RegisterCreationFunction<Square>();
	ObjectCreationRegistry::Get().RegisterCreationFunction<Triangle>();
};

class ReplicationManager
{
public:
	void ReplicateWorldState(OutputMemoryBitStream& inStream,
		const vector<GameObject*>& inAllObjects);

	void ReceiveReplicatedObjects(InputMemoryBitStream& inStream);

private:
	void ReplicateIntoStream(OutputMemoryBitStream& inStream,
		GameObject* inGameObject);

	GameObject* ReceiveReplicatedObject(InputMemoryBitStream& inStream);

	unordered_set<GameObject*> mObjectsReplicatedToMe;

	LinkingContext* mLinkingContext;
};

void ReplicationManager::ReplicateIntoStream(
	OutputMemoryBitStream& inStream,
	GameObject* inGameObject)
{
	//write game object id
	inStream.Write(mLinkingContext->GetNetworkId(inGameObject, true));

	//write game object class
	inStream.Write(inGameObject->GetClassId());

	//write game object data
	inGameObject->Write(inStream);
}

void ReplicationManager::ReplicateWorldState(
	OutputMemoryBitStream& inStream,
	const vector<GameObject*>& inAllObjects)
{
	//tag as replication data
	inStream.WriteBits(PT_ReplicationData, GetRequiredBits<PT_MAX>::Value);

	//write each object
	for (GameObject* go : inAllObjects)
	{
		ReplicateIntoStream(inStream, go);
	}
}

void ReplicationManager::ReceiveReplicatedObjects(
	InputMemoryBitStream& inStream)
{
	unordered_set<GameObject*> receivedObjects;

	while (inStream.GetRemainingBitCount() > 0)
	{
		GameObject* receivedGameObject = ReceiveReplicatedObject(inStream);
		receivedObjects.insert(receivedGameObject);
	}

	//now run through mObjectsReplicatedToMe.
	//if an object isn't in the recently replicated set,
	//destroy it
	for (GameObject* go : mObjectsReplicatedToMe)
	{
		if (receivedObjects.find(go) != receivedObjects.end())
		{
			mLinkingContext->RemoveGameObject(go);
			//go->Destroy();
		}
	}

	mObjectsReplicatedToMe = receivedObjects;
}

GameObject* ReplicationManager::ReceiveReplicatedObject(
	InputMemoryBitStream& inStream)
{
	uint32_t networkId;
	uint32_t classId;
	inStream.Read(networkId);
	inStream.Read(classId);

	GameObject* go = mLinkingContext->GetGameObject(networkId);
	if (!go)
	{
		go = ObjectCreationRegistry::Get().CreateGameObject(classId);
		mLinkingContext->AddGameObject(go, networkId);
	}

	//now read update
	go->Read(inStream);

	//return gameobject so we can track it was received in packet
	return go;
}

enum ReplicationAction
{
	RA_Create,
	RA_Update,
	RA_Destroy,
	RA_MAX
};

class ReplicationHeader
{
public:
	ReplicationHeader() {}

	ReplicationHeader(ReplicationAction inRA, uint32_t inNetworkId,
		uint32_t inClassId = 0) :
		mReplicationAction(inRA),
		mNetworkId(inNetworkId),
		mClassId(inClassId)
	{}

	ReplicationAction mReplicationAction;
	uint32_t mNetworkId;
	uint32_t mClassId;

	void Write(OutputMemoryBitStream& inStream);
	void Read(InputMemoryBitStream& inStream);
};

void ReplicationHeader::Write(OutputMemoryBitStream& inStream)
{
	inStream.WriteBits(mReplicationAction, GetRequiredBits<RA_MAX>::Value);
	inStream.Write(mNetworkId);
	if (mReplicationAction != RA_Destroy)
	{
		inStream.Write(mClassId);
	}
}

void ReplicationHeader::Read(InputMemoryBitStream& inStream)
{
	inStream.Read(mReplicationAction, GetRequiredBits<RA_MAX>::Value);
	inStream.Read(mNetworkId);
	if (mReplicationAction != RA_Destroy)
	{
		inStream.Read(mClassId);
	}
};


void DoTcpServer(std::string port)
{
	// Create socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	//listenSocket->SetNonBlockingMode(true);

	LOG("%s", "Listening socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080");
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating listen address");
		ExitProcess(1);
	}

	if (listenSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound listening socket");

	// Blocking function call -> Waits for some input; halts the program until something "interesting" happens
	// Non-Blocking function call -> Returns right away, as soon as the action is completed

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening on socket");

	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake

	LOG("%s", "Waiting to accept connections...");
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
	while (connSocket == nullptr)
	{
		connSocket = listenSocket->Accept(incomingAddress);
		// SocketUtil::ReportError("Accepting connection");
		// ExitProcess(1);
	}

	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());

	// Send & Receive Messages
	bool quit = false;
	bool sendQuit = false;

	std::thread sendThreadServer([&]() { // don't use [&] :)

		while (!sendQuit) // Need to add a quit here to have it really exit!
		{
			std::string msg;

			std::getline(std::cin, msg);

			if (msg == "/exit")
			{
				sendQuit = true;
				//connSocket->~TCPSocket();
				ExitProcess(1);
				break;
			}
			else
			{
				connSocket->Send(msg.c_str(), msg.length());
				std::cout << std::endl;
			}
		}
		});

	std::thread receiveThreadServer([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			char buffer[4096];
			int32_t bytesReceived = connSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				ExitProcess(1);
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				return;
			}

			std::string receivedMsg(buffer, bytesReceived);

			if (receivedMsg == "/exit")
			{
				ExitProcess(1);
			}
			else
			{
				std::cout << std::endl;
				std::cout << "\n[" << incomingAddress.ToString().c_str() << "] " << receivedMsg << std::endl;
				std::cout << std::endl;
			}
		}
		});

	/*std::cout << "Press enter to exit at any time!\n";
	std::cin.get();
	quit = true;
	connSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!*/
	receiveThreadServer.join();
	sendThreadServer.join();
}

void DoTcpClient(std::string port)
{
	// Create socket
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	LOG("%s", "Client socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	std::string address = StringUtils::Sprintf("127.0.0.1:%s", port.c_str());
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
	}

	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (clientSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}

	LOG("%s", "Connected to server!");

	bool quit = false;
	bool sendQuit = false;

	std::thread sendThreadClient([&]() { // don't use [&] :)

		while (!sendQuit) // Need to add a quit here to have it really exit!
		{
			std::string msg;
			std::getline(std::cin, msg);

			if (msg == "/exit")
			{
				sendQuit = true;
				//clientSocket->~TCPSocket();
				clientSocket->Send(msg.c_str(), msg.length());
				ExitProcess(1);
				break;
			}
			else
			{
				clientSocket->Send(msg.c_str(), msg.length());
				std::cout << std::endl;
			}
		}
		});

	std::thread receiveThreadClient([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			char buffer[4096];
			int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				ExitProcess(1);
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				return;
			}

			std::string receivedMsg(buffer, bytesReceived);

			if (receivedMsg == "/exit")
			{
				ExitProcess(1);
			}
			else
			{
				std::cout << std::endl;
				std::cout << "\n[" << servAddress->ToString() << "] " << receivedMsg << std::endl;
				std::cout << std::endl;
			}
		}
		});

	receiveThreadClient.join();
	sendThreadClient.join();
}

int main(int argc, const char** argv)
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;
#endif


	SocketUtil::StaticInit();

	//DoThreadExample();


	bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	if (isServer)
	{
		// Server code ----------------
		DoTcpServer(StringUtils::GetCommandLineArg(2));
	}
	else
	{
		// Client code ----------------
		DoTcpClient(StringUtils::GetCommandLineArg(2));
	}


	SocketUtil::CleanUp();

	return 0;
}



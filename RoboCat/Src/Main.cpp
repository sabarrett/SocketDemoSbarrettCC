
#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"
#include "allegro_wrapper_functions-main/Colour.h"
#include "allegro_wrapper_functions-main/InputSystem.h"

#include <thread>
#include <vector>
#include <map>

using namespace std;

#if _WIN32

enum PacketTypes
{
	CREATE,
	UPDATE,
	DESTROY
};

enum ObjectTypes
{
	LEFT,
	RIGHT,
	UP
};

UDPSocketPtr CreateBoundSocket(std::string IP)
{
	UDPSocketPtr cliSock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
		SocketAddressPtr cliAddr = SocketAddressFactory::CreateIPv4FromString(IP);
		if (cliAddr == nullptr)
		{
			SocketUtil::ReportError("Create Client Socket");
			ExitProcess(1);
		}
		cliSock->Bind(*cliAddr);
		return cliSock;
}

class LeftGameObject
{
	private:
		int xPos;
		int	yPos;
		string imageName;
	public:
		LeftGameObject(int x, int y, string fileName) { xPos = x; yPos = y; imageName = fileName; };
		int getX() { return xPos; };
		int getY() { return yPos; };
		void Update() { xPos--; };
		string getImageName() { return imageName; };
		void setX(int x) { xPos = x; };
		void setY(int y) { yPos = y; };
};

class GameState
{
	private:
		bool isServer;
		UDPSocketPtr sock;
		SocketAddress otherAddr;
		GraphicsLibrary mLibrary;

		map<int, LeftGameObject*> mLeftGameObjects;
		int leftObjcounter = 0;
	public:
		GameState()
		{
		}
		GameState(bool serverState, string ourAddr, string other)
		{
			isServer = serverState;
			UDPSocketPtr sock = CreateBoundSocket(ourAddr);
			SocketAddress otherAddr;
			{
				SocketAddressPtr tempAddr = SocketAddressFactory::CreateIPv4FromString(other);
				otherAddr = *tempAddr;
			}
			mLibrary = GraphicsLibrary(800, 600);
			mLibrary.init("../2517597.jpg");
		}
		~GameState()
		{
		}
		void Update()
		{
			InputSystem mInputSystem;
			mInputSystem.init(&mLibrary);

			mLibrary.loadImage("../2517597.jpg", "background");
			mLibrary.drawImage("background", 0, 0);
			mLibrary.render();

			bool escapePressed = false;

			while (!escapePressed)
			{
				for (auto& item : mLeftGameObjects)
				{
					LeftGameObject* obj = item.second;
					if (isServer)
					{
						obj->Update();
						SendPacket(sock, otherAddr, PacketTypes::UPDATE, ObjectTypes::LEFT, obj->getX(), obj->getY(), item.first);
					}
					ReceivePacket(sock, otherAddr);
					mLibrary.drawImage("LeftObject", obj->getX(), obj->getY());
				}
				int mousePress = mInputSystem.getMouseInput();
				int keyPress = mInputSystem.getKeyboardInput();

				if (keyPress == KeyCode::KeyEscape)
					escapePressed = true;

				switch (mousePress)
				{
				case MouseButton::LeftMouse:
				{
					mLibrary.loadImage("../output-onlinepngtools.png", "LeftObject");
					mLeftGameObjects.emplace(leftObjcounter++, new LeftGameObject((int)mInputSystem.getMouseX(), (int)mInputSystem.getMouseY(), "../output-onlinepngtools.png"));
					break;
				}
				}
				mLibrary.render();
				mLibrary.drawImage("background", 0, 0);
			}
		}
		void SendPacket(UDPSocketPtr ptr, SocketAddress addr, int packetType, int objectType,
			int pos_x, int pos_y, int ID)
		{
			int packet[5];
			packet[0] = packetType;
			packet[1] = objectType;
			packet[2] = ID;
			packet[3] = pos_x;
			packet[4] = pos_y;

			char* bytePacket = (char*)packet;

			ptr->SendTo(bytePacket, 20, addr);
		}
		void ReceivePacket(UDPSocketPtr ptr, SocketAddress addr)
		{
			char buffer[20];
			while (true)
			{
				int bytesReceived = ptr->ReceiveFrom(buffer, 20, addr);
				if (bytesReceived < 0)
				{
					SocketUtil::ReportError("Error receiving packet");
					break;
				}
				else if (bytesReceived == 0)
					break;
				else
				{
					int* packet = (int*)buffer;
					switch (packet[0])
					{
						case PacketTypes::CREATE:
						{
							break;
						}
						case PacketTypes::DESTROY:
						{
							break;
						}
						case PacketTypes::UPDATE:
						{
							switch (packet[1])
							{
								case ObjectTypes::LEFT:
								{
									LeftGameObject* obj = mLeftGameObjects.find(packet[2])->second;
									obj->setX(packet[3]);
									obj->setY(packet[4]);
									break;
								}
								case ObjectTypes::RIGHT:
								{
									break;
								}
								case ObjectTypes::UP:
								{
									break;
								}
							}
							break;
						}
					}
				}
			}
		}
};

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
	GameState state;

	bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	if (isServer)
	{
		state = GameState(true, "127.0.0.1:9000", "127.0.0.1:9001");
	}
	else
	{
		state = GameState(true, "127.0.0.1:9001", "127.0.0.1:9000");
	}

	state.Update();

	cin.get();

	SocketUtil::CleanUp();

	return 0;
}

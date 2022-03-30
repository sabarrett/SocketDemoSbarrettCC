
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
	DESTROY,
	HELLO
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

class PlayerGameObject
{
	private:
		int xPos;
		int	yPos;
		string imageName;
		bool isPlayer1;
	public:
		PlayerGameObject(int x, int y, string fileName, bool isPlayer) 
		{ 
			xPos = x; 
			yPos = y; 
			imageName = fileName; 
			isPlayer1 = isPlayer;
		};
		int getX() { return xPos; };
		int getY() { return yPos; };
		void Update(int keyPress) 
		{
			if (isPlayer1)
			{
				switch (keyPress)
				{
				case KeyCode::P1_Down:
					yPos += 7;
					break;
				case KeyCode::P1_Left:
					xPos -= 7;
					break;
				case KeyCode::P1_Right:
					xPos += 7;
					break;
				case KeyCode::P1_Up:
					yPos -= 7;
					break;
				}
			}
			else
			{
				switch (keyPress)
				{
				case KeyCode::P2_Down:
					yPos += 3;
					break;
				case KeyCode::P2_Left:
					xPos -= 3;
					break;
				case KeyCode::P2_Right:
					xPos += 3;
					break;
				case KeyCode::P2_Up:
					yPos -= 3;
					break;
				}
			}
		};
		string getImageName() { return imageName; };
		void setX(int x) { xPos = x; };
		void setY(int y) { yPos = y; };
};

class CoinObject
{
	private:
		int xPos, yPos;
	public:
		CoinObject(int x, int y) { xPos = x; yPos = y; };
		int getX() { return xPos; };
		int getY() { return yPos; };
};

class TomatoObject
{
	private:
		int xPos, yPos;
	public:
		TomatoObject(int x, int y) { xPos = x; yPos = y; };
		int getX() { return xPos; };
		int getY() { return yPos; };
		void Update() { xPos--; };
};

class GameState
{
	private:
		bool isServer;
		UDPSocketPtr sock;
		SocketAddress otherAddr;
		GraphicsLibrary* mLibrary;
		InputSystem* mInputSystem;
		PlayerGameObject* mPlayer;
		map<string, CoinObject*> mCoins;
		TomatoObject* tomato;
	public:
		GameState(bool serverState, string ourAddr, string other)
		{
			cout << other << endl;
			isServer = serverState;
			sock = CreateBoundSocket(ourAddr);
			//if (isServer)
				sock->SetNonBlockingMode(true);
			otherAddr = *(SocketAddressFactory::CreateIPv4FromString(other));
			cout << otherAddr.ToString() << endl;
			mLibrary = new GraphicsLibrary(800, 600);
			mLibrary->init("../2517597.jpg");
			mInputSystem = new InputSystem();
			mInputSystem->init(mLibrary);
			if (isServer)
				mPlayer = new PlayerGameObject(650, 100, "../kirby.png", isServer);
			else
				mPlayer = new PlayerGameObject(100, 100, "../homie.png", isServer);
		}
		~GameState()
		{
			delete mLibrary;
		}
		void Update()
		{
			cout << otherAddr.ToString() << endl;
			mLibrary->loadImage("../2517597.jpg", "background");
			mLibrary->loadImage(mPlayer->getImageName(), "Player");
			mLibrary->loadImage("../coin.png", "coin");
			mLibrary->loadImage("../tomato.png", "tomato");
			mLibrary->drawImage("background", 0, 0);
			mLibrary->drawImage("Player", mPlayer->getX(), mPlayer->getY());

			if (isServer)
			{
				tomato = new TomatoObject(800, rand() % 540 + 20);
				for (int i = 0; i < 10; i++)
					mCoins.push_back(new CoinObject(rand() % 700 + 50, rand() % 500 + 50));
			}
			else
			{
				SendPacket(sock, &otherAddr, PacketTypes::HELLO, 1, 1, 1, 1);
			}

			mLibrary->render();


			bool escapePressed = false;

			while (!escapePressed)
			{
				if (isServer)
				{
					tomato->Update();
					CheckForCollisions();
				}
				ReceivePacket(sock, &otherAddr, mInputSystem);
				for (auto& coin : mCoins)
				{
					mLibrary->drawImage("coin", coin.second->getX(), coin.second->getY());
				}
				//mLibrary->drawImage("tomato", tomato->getX(), tomato->getY());

				int keyPress = mInputSystem->getKeyboardInput();
				mPlayer->Update(keyPress);
				mLibrary->drawImage("Player", mPlayer->getX(), mPlayer->getY());

				if (keyPress == KeyCode::KeyEscape)
					escapePressed = true;

				mLibrary->render();
				mLibrary->drawImage("background", 0, 0);
			}
		}
		void SendPacket(UDPSocketPtr ptr, SocketAddress* addr, int packetType, int objectType,
			int pos_x, int pos_y, int ID)
		{
			cout << addr->ToString() << endl;
			int packet[5];
			packet[0] = packetType;
			packet[1] = objectType;
			packet[2] = ID;
			packet[3] = pos_x;
			packet[4] = pos_y;

			char* bytePacket = (char*)packet;

			cout << "attempting to send packet" << endl;

			int bytesSent = ptr->SendTo(bytePacket, 20, *addr);
			cout << bytesSent << endl;
			if (bytesSent <= 0)
			{
				SocketUtil::ReportError("Client SendTo");
			}

			cout << "Sent packet" << endl;
		}
		void ReceivePacket(UDPSocketPtr ptr, SocketAddress* addr, InputSystem* mInputSystem)
		{
			char buffer[20];
			while (true)
			{
				int bytesReceived = ptr->ReceiveFrom(buffer, 20, *addr);
				if (bytesReceived < 0)
				{
					SocketUtil::ReportError("Error receiving packet");
					break;
				}
				else if (bytesReceived == 0)
					break;
				else
				{
					cout << "received packet" << endl;
					int* packet = (int*)buffer;
					switch (packet[0])
					{
						case PacketTypes::CREATE:
						{
							//mLeftGameObjects.emplace(leftObjcounter++, new LeftGameObject((int)mInputSystem->getMouseX(), (int)mInputSystem->getMouseY(), "../output-onlinepngtools.png"));
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
									//LeftGameObject* obj = mLeftGameObjects.find(packet[2])->second;
									//obj->setX(packet[3]);
									//obj->setY(packet[4]);
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
						case PacketTypes::HELLO:
						{
							cout << "hello packet received!" << endl;
							break;
						}
					}
				}
			}
		}

		void CheckForCollisions()
		{
			for (auto& coin : mCoins)
			{
				if (((mPlayer->getX() < coin.second->getX() && mPlayer->getX() + 100 > coin.second->getX())//top left corner
					&& (mPlayer->getY() < coin.second->getY() && mPlayer->getY() + 100 > coin.second->getY()))

					|| ((mPlayer->getX() < coin.second->getX() + 50 && mPlayer->getX() + 100 > coin.second->getX() + 50)//bottom right
					&& (mPlayer->getY() < coin.second->getY() + 50 && mPlayer->getY() + 100 > coin.second->getY() + 50))

					|| ((mPlayer->getX() < coin.second->getX() + 50 && mPlayer->getX() + 100 > coin.second->getX() + 50)//top right
						&& (mPlayer->getY() < coin.second->getY() && mPlayer->getY() + 100 > coin.second->getY()))

						|| ((mPlayer->getX() < coin.second->getX() && mPlayer->getX() + 100 > coin.second->getX())//bottom left
							&& (mPlayer->getY() < coin.second->getY() + 50 && mPlayer->getY() + 100 > coin.second->getY() + 50)))
				{
					mCoins.erase(coin.first);
					delete coin.second;
					mCoins.insert(coin.first, new CoinObject(rand() % 700 + 50, rand() % 500 + 50));
					break;
				}
			}
			if (((mPlayer->getX() < tomato->getX() && mPlayer->getX() + 100 > tomato->getX())//top left corner
				&& (mPlayer->getY() < tomato->getY() && mPlayer->getY() + 100 > tomato->getY()))

				|| ((mPlayer->getX() < tomato->getX() + 30 && mPlayer->getX() + 100 > tomato->getX() + 30)//bottom right
					&& (mPlayer->getY() < tomato->getY() + 30 && mPlayer->getY() + 100 > tomato->getY() + 30))

				|| ((mPlayer->getX() < tomato->getX() + 30 && mPlayer->getX() + 100 > tomato->getX() + 30)//top right
					&& (mPlayer->getY() < tomato->getY() && mPlayer->getY() + 100 > tomato->getY()))

				|| ((mPlayer->getX() < tomato->getX() && mPlayer->getX() + 100 > tomato->getX())//bottom left
					&& (mPlayer->getY() < tomato->getY() + 30 && mPlayer->getY() + 100 > tomato->getY() + 30))
				
				|| tomato->getX() <= 0)
			{
				tomato = new TomatoObject(800, rand() % 540 + 20);
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
	srand(time(NULL));
	SocketUtil::StaticInit();

	//DoThreadExample();
	GameState* state;

	bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	if (isServer)
	{
		state = new GameState(true, "localhost:9000", "localhost:9001");
	}
	else
	{
		state = new GameState(false, "localhost:9001", "localhost:9000");
	}

	state->Update();

	//cin.get();

	SocketUtil::CleanUp();

	delete state;

	return 0;
}

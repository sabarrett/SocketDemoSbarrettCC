
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
	PLAYER,
	COIN,
	TOMATO
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
		PlayerGameObject* otherPlayer;
		vector<CoinObject*> vCoins;
		TomatoObject* tomato;
	public:
		GameState(bool serverState, string ourAddr, string other)
		{
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
			{
				mLibrary->loadImage("../kirby.png", "Player");
				mLibrary->loadImage("../homie.png", "Other Player");
				mPlayer = new PlayerGameObject(650, 100, "../kirby.png", isServer);
				otherPlayer = new PlayerGameObject(100, 100, "../homie.png", isServer);
			}
			else
			{
				mLibrary->loadImage("../kirby.png", "Other Player");
				mLibrary->loadImage("../homie.png", "Player");
				otherPlayer = new PlayerGameObject(650, 100, "../kirby.png", isServer);
				mPlayer = new PlayerGameObject(100, 100, "../homie.png", isServer);
			}
		}
		~GameState()
		{
			delete mLibrary;
		}
		void Update()
		{
			mLibrary->loadImage("../2517597.jpg", "background");
			mLibrary->loadImage("../coin.png", "coin");
			mLibrary->loadImage("../tomato.png", "tomato");
			mLibrary->drawImage("background", 0, 0);
			mLibrary->drawImage("Player", mPlayer->getX(), mPlayer->getY());
			mLibrary->drawImage("Other Player", otherPlayer->getX(), otherPlayer->getY());

			if (isServer)
			{
				tomato = new TomatoObject(800, rand() % 540 + 20);
				for (int i = 0; i < 10; i++)
					vCoins.push_back(new CoinObject(rand() % 700 + 50, rand() % 500 + 50));
			}
			else
			{
				SendPacket(sock, &otherAddr, PacketTypes::HELLO, 1, 1, 1);
			}

			mLibrary->render();


			bool escapePressed = false;

			while (!escapePressed)
			{
				if (isServer)
				{
					tomato->Update();
					CheckForCollisions(mPlayer);
					CheckForCollisions(otherPlayer);
				}
				SendPacket(sock, &otherAddr, PacketTypes::UPDATE, ObjectTypes::PLAYER, mPlayer->getX(), mPlayer->getY());
				ReceivePacket(sock, &otherAddr, mInputSystem);
				for (auto& coin : vCoins)
				{
					mLibrary->drawImage("coin", coin->getX(), coin->getY());
				}
				//mLibrary->drawImage("tomato", tomato->getX(), tomato->getY());

				int keyPress = mInputSystem->getKeyboardInput();
				mPlayer->Update(keyPress);
				mLibrary->drawImage("Player", mPlayer->getX(), mPlayer->getY());
				mLibrary->drawImage("Other Player", otherPlayer->getX(), otherPlayer->getY());
				cout << otherPlayer->getX() << ", " << otherPlayer->getY() << endl;

				if (keyPress == KeyCode::KeyEscape)
					escapePressed = true;

				mLibrary->render();
				mLibrary->drawImage("background", 0, 0);
			}
		}
		void SendPacket(UDPSocketPtr ptr, SocketAddress* addr, int packetType, int objectType,
			int pos_x, int pos_y)
		{
			int packet[25];
			packet[0] = packetType;
			packet[1] = objectType;
			packet[2] = pos_x;
			packet[3] = pos_y;

			char* bytePacket = (char*)packet;

			int bytesSent = ptr->SendTo(bytePacket, 100, *addr);
			cout << bytesSent << endl;
			if (bytesSent <= 0)
			{
				SocketUtil::ReportError("Client SendTo");
			}
		}
		void SendCoinPacket(UDPSocketPtr ptr, SocketAddress* addr, int packetType, int objectType, vector<CoinObject*> coins)
		{
			int packet[25];
			packet[0] = packetType;
			packet[1] = objectType;
			for (int i = 2; i < 20; i+=2)
			{
				packet[i] = coins[i/2 - 1]->getX();
				packet[i+1] = coins[i/2 - 1]->getY();
			}

			char* bytePacket = (char*)packet;

			int bytesSent = ptr->SendTo(bytePacket, 100, *addr);
			if (bytesSent <= 0)
			{
				SocketUtil::ReportError("Client SendTo");
			}
		}
		void ReceivePacket(UDPSocketPtr ptr, SocketAddress* addr, InputSystem* mInputSystem)
		{
			char buffer[100];
			//while (true)
			{
				int bytesReceived = ptr->ReceiveFrom(buffer, 100, *addr);
				if (bytesReceived < 0)
				{
					SocketUtil::ReportError("Error receiving packet");
					//break;
				}
				else if (bytesReceived == 0)
					//break;
					return;
				else
				{
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
								case ObjectTypes::PLAYER:
								{
									otherPlayer->setX(packet[2]);
									otherPlayer->setY(packet[3]);
									break;
								}
								case ObjectTypes::COIN:
								{
									vCoins.clear();
									for (int i = 2; i < 20; i += 2)
									{
										vCoins.push_back(new CoinObject(packet[i], packet[i + 1]));
										//vCoins[i / 2 - 1] = new CoinObject(packet[i], packet[i + 1]);
									}
									break;
								}
								case ObjectTypes::TOMATO:
								{
									break;
								}
							}
							break;
						}
						case PacketTypes::HELLO:
						{
							SendCoinPacket(sock, &otherAddr, PacketTypes::UPDATE, ObjectTypes::COIN, vCoins);
							break;
						}
					}
				}
			}
		}

		void CheckForCollisions(PlayerGameObject* player)
		{
			for (vector<CoinObject*>::iterator it = vCoins.begin(); it < vCoins.end(); it++)
			{
				CoinObject* coin = *it;
				if (((player->getX() < coin->getX() && player->getX() + 100 > coin->getX())//top left corner
					&& (player->getY() < coin->getY() && player->getY() + 100 > coin->getY()))

					|| ((player->getX() < coin->getX() + 50 && player->getX() + 100 > coin->getX() + 50)//bottom right
					&& (player->getY() < coin->getY() + 50 && player->getY() + 100 > coin->getY() + 50))

					|| ((player->getX() < coin->getX() + 50 && player->getX() + 100 > coin->getX() + 50)//top right
						&& (player->getY() < coin->getY() && player->getY() + 100 > coin->getY()))

						|| ((player->getX() < coin->getX() && player->getX() + 100 > coin->getX())//bottom left
							&& (player->getY() < coin->getY() + 50 && player->getY() + 100 > coin->getY() + 50)))
				{
					vCoins.erase(it);
					delete coin;
					vCoins.push_back(new CoinObject(rand() % 700 + 50, rand() % 500 + 50));
					SendCoinPacket(sock, &otherAddr, PacketTypes::UPDATE, ObjectTypes::COIN, vCoins);
					break;
				}
			}
			if (((player->getX() < tomato->getX() && player->getX() + 100 > tomato->getX())//top left corner
				&& (player->getY() < tomato->getY() && player->getY() + 100 > tomato->getY()))

				|| ((player->getX() < tomato->getX() + 30 && player->getX() + 100 > tomato->getX() + 30)//bottom right
					&& (player->getY() < tomato->getY() + 30 && player->getY() + 100 > tomato->getY() + 30))

				|| ((player->getX() < tomato->getX() + 30 && player->getX() + 100 > tomato->getX() + 30)//top right
					&& (player->getY() < tomato->getY() && player->getY() + 100 > tomato->getY()))

				|| ((player->getX() < tomato->getX() && player->getX() + 100 > tomato->getX())//bottom left
					&& (player->getY() < tomato->getY() + 30 && player->getY() + 100 > tomato->getY() + 30))
				
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

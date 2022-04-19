#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"
#include "allegro_wrapper_functions-main/Colour.h"
#include "allegro_wrapper_functions-main/InputSystem.h"

#include <thread>
#include <vector>
#include <map>
#include <Windows.h>

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

class Packet
{
	private:
		int* buffer;
		long int timeStamp;

	public:
		Packet(int* b, long int time)
		{
			buffer = b;
			timeStamp = time;
		}

		long int getTimeStamp() { return timeStamp; };
		int* getBuffer() { return buffer; };

		friend bool operator<(const Packet& a, const Packet& b)
		{
			return a.timeStamp > b.timeStamp;
		};
};

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
					yPos += 7;
					break;
				case KeyCode::P2_Left:
					xPos -= 7;
					break;
				case KeyCode::P2_Right:
					xPos += 7;
					break;
				case KeyCode::P2_Up:
					yPos -= 7;
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
		double mTimeUntilNextFrame, msPerFrame = 1000/2;
	public:
		TomatoObject(int x, int y) { xPos = x; yPos = y; };
		int getX() { return xPos; };
		int getY() { return yPos; };
		void Update(double deltaTime) 
		{
			mTimeUntilNextFrame -= deltaTime;
			if (mTimeUntilNextFrame <= 0)
			{
				mTimeUntilNextFrame = msPerFrame;
				xPos--;
			}
		};
};

/* Timer - high accuracy timer - uses Large Integer to prevent rollover
Dean Lawson
Champlain College
2011
*/

class Timer
{

private:
	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mEndTime;
	LARGE_INTEGER mTimerFrequency;
	double mElapsedTime;
	double mFactor;
	double mLastFactor;
	bool mPaused;

	//helper function - uses current frequency for the Timer
	double calcDifferenceInMS(LARGE_INTEGER from, LARGE_INTEGER to) const
	{
		double difference = (double)(to.QuadPart - from.QuadPart) / (double)mTimerFrequency.QuadPart;
		difference *= mFactor;
		return difference * 1000;
	}
public:
	Timer()
		:mElapsedTime(0.0)
		, mPaused(true)
		, mFactor(1.0)
		, mLastFactor(1.0)
	{
		QueryPerformanceFrequency(&mTimerFrequency);
		mStartTime.QuadPart = 0;
		mEndTime.QuadPart = 0;
	}
	~Timer()
	{
	}

	void start()
	{
		QueryPerformanceCounter(&mStartTime);

		//reset end time as well
		mEndTime.QuadPart = 0;

		mElapsedTime = 0.0;

		pause(false);//unpause
	}
	void stop()
	{
		QueryPerformanceCounter(&mEndTime);
		mElapsedTime = calcDifferenceInMS(mStartTime, mEndTime);
	}
	double getElapsedTime() const//returns how much time has elapsed since start
	{
		//if we have an end time then the timer isn't running and we can just return the elapsed time
		if (mEndTime.QuadPart != 0)
		{
			return mElapsedTime;
		}
		else //otherwise we need to get the current time, do the math and return that
		{
			LARGE_INTEGER currentTime;
			QueryPerformanceCounter(&currentTime);
			return calcDifferenceInMS(mStartTime, currentTime);
		}
	}
	void sleepUntilElapsed(double ms)
	{
		LARGE_INTEGER currentTime, lastTime;
		QueryPerformanceCounter(&currentTime);
		double timeToSleep = ms - calcDifferenceInMS(mStartTime, currentTime);

		while (timeToSleep > 0.0)
		{
			lastTime = currentTime;
			QueryPerformanceCounter(&currentTime);
			double timeElapsed = calcDifferenceInMS(lastTime, currentTime);
			timeToSleep -= timeElapsed;
			if (timeToSleep > 10.0)//if we are going to be in this loop for a long time - 
			{						//Sleep to relinquish back to Windows
				Sleep(10);
			}
		}
	}
	void pause(bool shouldPause)
	{
		if (shouldPause && !mPaused)//want to pause and we are not currently paused
		{
			mPaused = true;
			QueryPerformanceCounter(&mEndTime);
			mElapsedTime += calcDifferenceInMS(mStartTime, mEndTime);
		}
		else if (!shouldPause && mPaused)//want to unpause and we are paused
		{
			mPaused = false;
			QueryPerformanceCounter(&mStartTime);
		}
	}
	inline double getFactor() const { return mFactor; };
	inline void multFactor(double mult) { mLastFactor = mFactor; mFactor *= mult; };
	inline void setFactor(double theFactor) { mLastFactor = mFactor; mFactor = theFactor; };
	inline void restoreLastFactor() { mFactor = mLastFactor; };

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
		double mTimePerFrame = 1000/2;
		long int timeBeforeProcessing = 250;
		priority_queue<Packet*> mReceivedPackets;
	public:
		GameState(bool serverState, string ourAddr, string other)
		{
			isServer = serverState;
			sock = CreateBoundSocket(ourAddr);
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
		//change receive method to return the packet, put the packet in a priority (?) queue of returned packets along with a time stamp, only process packets received x seconds ago
		void Update()//make time a part of this so that it runs the same rate on all systems
		{
			Timer timer;
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
				tomato = new TomatoObject(800, 800);
				SendPacket(sock, &otherAddr, PacketTypes::HELLO, 1, 1, 1);
			}
			mLibrary->render();
			bool escapePressed = false;

			while (!escapePressed)
			{
				timer.start();
				if (isServer)
				{
					tomato->Update(mTimePerFrame);
					SendPacket(sock, &otherAddr, PacketTypes::UPDATE, ObjectTypes::TOMATO, tomato->getX(), tomato->getY());
					SendCoinPacket(sock, &otherAddr, PacketTypes::UPDATE, ObjectTypes::COIN, vCoins);
					CheckForCollisions(mPlayer);
					CheckForCollisions(otherPlayer);
				}
				else
				{
					mReceivedPackets.push(new Packet(ReceivePacket(sock, &otherAddr), static_cast<long int>(time(NULL)) + rand() % 6 - 3));
				}
				SendPacket(sock, &otherAddr, PacketTypes::UPDATE, ObjectTypes::PLAYER, mPlayer->getX(), mPlayer->getY());
				int* temp = ReceivePacket(sock, &otherAddr);
				mReceivedPackets.push(new Packet(temp, static_cast<long int>(time(NULL)) + rand() % 6 - 3));

				while (!mReceivedPackets.empty() && static_cast <long int> (time(NULL)) - mReceivedPackets.top()->getTimeStamp() < timeBeforeProcessing)
				{
					Packet* tempPacket = mReceivedPackets.top();
					mReceivedPackets.pop();
					if (tempPacket != nullptr)
					{
						processPacket(tempPacket->getBuffer());
					}
					delete tempPacket;
				}

				for (auto& coin : vCoins)
				{
					mLibrary->drawImage("coin", coin->getX(), coin->getY());
				}

				mLibrary->drawImage("tomato", tomato->getX(), tomato->getY());
				int keyPress = mInputSystem->getKeyboardInput();
				mPlayer->Update(keyPress);
				mLibrary->drawImage("Player", mPlayer->getX(), mPlayer->getY());
				mLibrary->drawImage("Other Player", otherPlayer->getX(), otherPlayer->getY());
				if (keyPress == KeyCode::KeyEscape)
					escapePressed = true;

				mLibrary->render();
				mLibrary->drawImage("background", 0, 0);
				timer.sleepUntilElapsed(mTimePerFrame);
			}
		}
		void SendPacket(UDPSocketPtr ptr, SocketAddress* addr, int packetType, int objectType,
			int pos_x, int pos_y)
		{
			const int packetSize = sizeof(int) * 25;
			int packet[packetSize / sizeof(int)];
			packet[0] = packetType;
			packet[1] = objectType;
			packet[2] = pos_x;
			packet[3] = pos_y;

			{
				int bytesSent = ptr->SendTo((char*)packet, packetSize, *addr);
				if (bytesSent <= 0)
				{
					SocketUtil::ReportError("Client SendTo");
				}
			}
		}
		void SendCoinPacket(UDPSocketPtr ptr, SocketAddress* addr, int packetType, int objectType, vector<CoinObject*> coins)
		{
			int packet[25];
			packet[0] = packetType;
			packet[1] = objectType;
			for (int i = 2; i < 22; i += 2)
			{
				packet[i] = coins[i / 2 - 1]->getX();
				packet[i + 1] = coins[i / 2 - 1]->getY();
			}

			char* bytePacket = (char*)packet;
			{
				int bytesSent = ptr->SendTo(bytePacket, 100, *addr);
				if (bytesSent <= 0)
				{
					SocketUtil::ReportError("Client SendTo");
				}
			}
		}
		int* ReceivePacket(UDPSocketPtr ptr, SocketAddress* addr)
		{
			char buffer[100];
			{
				int bytesReceived = ptr->ReceiveFrom(buffer, 100, *addr);
				if (bytesReceived < 0)
				{
					SocketUtil::ReportError("Error receiving packet");
				}
				else if (bytesReceived == 0)
					return nullptr;
				else
				{
					char* packet = new char[100];
					for (int i = 0; i < 100; i++)
					{
						packet[i] = buffer[i];
					}
					int* pac = (int*)packet;
					cout << pac[2] << ", " << pac[3] << endl;
					return pac;
				}
			}
			return nullptr;
		}

		void processPacket(int* packet)
		{
			if (packet != nullptr)
			{
				cout << packet[0] << endl;
				switch (packet[0])
				{
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
						cout << "coin packet received" << endl;
						vCoins.clear();
						for (int i = 2; i < 22; i += 2)
						{
							vCoins.push_back(new CoinObject(packet[i], packet[i + 1]));
						}
						cout << vCoins.size() << endl;
						break;
					}
					case ObjectTypes::TOMATO:
					{
						tomato = new TomatoObject(packet[2], packet[3]);
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

	SocketUtil::CleanUp();

	delete state;

	return 0;
}

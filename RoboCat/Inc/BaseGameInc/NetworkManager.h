#pragma once
#include "Trackable.h"
#include "RoboCatPCH.h"
#include "Unit.h"
#include "Game.h"
class NetworkManager : public Trackable
{
public:
	//sent when first trying to join
	static const uint32_t	kHelloCC = 'HELO';
	//sent when accepted by master peer
	static const uint32_t	kWelcomeCC = 'WLCM';
	//sent as a reply to HELO if it isn't the master peer
	static const uint32_t	kNotMasterPeerCC = 'NOMP';
	//sent as a reply to HELO if the game can't be joined (either full or already started)
	static const uint32_t	kNotJoinableCC = 'NOJN';
	//sent by new player to all non-master peers after being accepted
	static const uint32_t	kIntroCC = 'INTR';
	//contains data for a particular turn
	static const uint32_t	kUpdateCC = 'UPDT';
	//notifies peers that the game will be starting soon
	static const uint32_t	kStartCC = 'STRT';
	static const int		kMaxPacketsPerFrameCount = 10;

	enum class NetworkManagerState
	{
		NMS_Unitialized,
		NMS_Hello,
		NMS_Lobby,
		//everything above this should be the pre-game/lobby/connection
		NMS_Starting,
		NMS_Playing,
		NMS_Delay,
	};

	
	//static bool	StaticInitAsMasterPeer(uint16_t inPort, const string& inName);
	//static bool StaticInitAsPeer(const SocketAddress& inMPAddress, const string& inName);

	NetworkManager();
	~NetworkManager();

	void cleanup();

	void	ProcessIncomingPackets();

	void	SendOutgoingPackets();

private:
	void	UpdateSayingHello(bool inForce = false);
	void	SendHelloPacket();
	void	UpdateStarting();
	void	UpdateSendActionPacket();

public:

	void	ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
private:
	void	ProcessPacketsHello(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
	void	HandleNotMPPacket(InputMemoryBitStream& inInputStream);
	void	HandleWelcomePacket(InputMemoryBitStream& inInputStream);
	void	ProcessPacketsLobby(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
	void	HandleHelloPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
	void	HandleIntroPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
	void	HandleStartPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
	void	HandleUpdatePacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
	void	ProcessPacketsPlaying(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
	void	ProcessPacketsDelay(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);

public:
	void	HandleConnectionReset(const SocketAddress& inFromAddress);

	void	SendPacket(const OutputMemoryBitStream& inOutputStream, const SocketAddress& inToAddress);

	void	TryStartGame();

	const WeightedTimedMovingAverage& GetBytesReceivedPerSecond()	const { return mBytesReceivedPerSecond; }
	const WeightedTimedMovingAverage& GetBytesSentPerSecond()		const { return mBytesSentPerSecond; }

	void	SetDropPacketChance(float inChance) { mDropPacketChance = inChance; }
	float	GetDropPacketChance() const { return mDropPacketChance; }
	void	SetSimulatedLatency(float inLatency) { mSimulatedLatency = inLatency; }
	float	GetSimulatedLatency() const { return mSimulatedLatency; }

	bool	IsMasterPeer() const { return mIsMasterPeer; }
	float	GetTimeToStart() const { return mTimeToStart; }

	int		GetPlayerCount() const { return mPlayerCount; }
	uint32_t GetMyPlayerId() const { return mPlayerId; }

	bool	InitAsMasterPeer(uint16_t inPort, const string& inName);
	bool	InitAsPeer(const SocketAddress& inMPAddress, const string& inName);
	bool	InitSocket(uint16_t inPort);

	NetworkManagerState GetState() { return mState; };

	void addAction(Game::ActionTypes type, Vector2D pos, int seed);

private:
	
	class ReceivedPacket
	{
	public:
		ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, const SocketAddress& inAddress);

		const	SocketAddress& GetFromAddress()	const { return mFromAddress; }
		float	GetReceivedTime()	const { return mReceivedTime; }
		InputMemoryBitStream& GetPacketBuffer() { return mPacketBuffer; }

	private:

		float					mReceivedTime;
		InputMemoryBitStream	mPacketBuffer;
		SocketAddress			mFromAddress;

	};

	struct ActionData
	{
		void Write(OutputMemoryBitStream& inOutputStream);
		void Read(InputMemoryBitStream& inInputStream);
		Game::ActionTypes type;
		Vector2D postion;
		int seed;
	};

	void	UpdateBytesSentLastFrame();
	void	ReadIncomingPacketsIntoQueue();
	void	ProcessQueuedPackets();

	void	UpdateHighestPlayerId(uint32_t inId);
	void	EnterPlayingState();

	//these should stay ordered!
	typedef map< uint32_t, SocketAddress > IntToSocketAddrMap;
	typedef map< uint32_t, string > IntToStrMap;
	
	//Change to Unit pointer?
	typedef map< uint32_t, Unit* > IntToGameObjectMap;
	typedef unordered_map< SocketAddress, uint32_t > SocketAddrToIntMap;

	
	queue< ReceivedPacket, list< ReceivedPacket > >	mPacketQueue;

	IntToGameObjectMap			mNetworkIdToGameObjectMap;
	IntToSocketAddrMap			mPlayerToSocketMap;
	SocketAddrToIntMap			mSocketToPlayerMap;

	IntToStrMap					mPlayerNameMap;

	UDPSocketPtr	mSocket;
	SocketAddress	mMasterPeerAddr;

	
	int mBytesSentThisFrame;
	std::string	mName;
	WeightedTimedMovingAverage	mBytesReceivedPerSecond;
	WeightedTimedMovingAverage	mBytesSentPerSecond;

	float			mDropPacketChance;
	float			mSimulatedLatency;

	float			mTimeOfLastHello;
	float			mTimeToStart;

	int				mPlayerCount;
	//we track the highest player id seen in the event
	//the master peer d/cs and we need a new master peer
	//who can assign ids
	uint32_t		mHighestPlayerId;
	uint32_t		mNewNetworkId;
	uint32_t		mPlayerId;

	bool			mIsMasterPeer;

	NetworkManagerState	mState;

	vector<ActionData> mActionVec;
};


#pragma once
#include "Util/RoboCatPCH.h"
#include "Util/NetworkManager.h"

class Game;

class NetworkManagerClient : public NetworkManager
{
public:
	NetworkManagerClient();
	void	Init(Game* game);
	void	ConnectToServer(std::string address, std::string port);

	void	SendOutgoingPackets();

	virtual void	ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) override;
	virtual void	HandleConnectionReset(const SocketAddress& inFromAddress) override;

	const	WeightedTimedMovingAverage& GetAvgRoundTripTime()	const { return mAvgRoundTripTime; }
	float									GetRoundTripTime()		const { return mAvgRoundTripTime.GetValue(); }
	int		GetPlayerId()											const { return m_myPlayerUID; }

	std::vector<GameObject*> getAllObjectsVector() { return m_allObjects; }


private:

	void	HandleWorldStatePacket(InputMemoryBitStream* inStream);
	void	ReceiveTCP();
	virtual void	ReadIncomingPacketsIntoQueue() override;

	DeliveryNotificationManager	mDeliveryNotificationManager;

	WeightedTimedMovingAverage	mAvgRoundTripTime;
	float						mLastRoundTripTime;

	SocketAddressPtr m_serverAddress;

	Game* m_game;

	int m_myPlayerUID; // UID of the object move commands from this client will target

};
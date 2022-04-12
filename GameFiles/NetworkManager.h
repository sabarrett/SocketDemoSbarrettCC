#pragma once

#include "GameFiles/JoinerInput.h"
#include "WorldState.h"

namespace NetworkManager
{
    // can probably be bigger since we're using UDP
    const int BUFFER_SIZE = 4096;
    const int CONNECTION_TIMOUT = 7 * 1000; // * 1000 converts to milliseconds
    const int PACKET_DROP_CHANCE_PERCENT = 20; // goes from 0 to 100
    const int PACKET_DELAY_CHANCE_PERCENT = 5; // goes from 0 to 100
    const int PACKET_DELAY_TIME = 1000 * 1.0; //delay of 1.0 second

    const string HOME_ADDRESS = "127.0.0.1:";
    const string ACCEPT_ALL_ADDRESS = "0.0.0.0:";

    static time_t timeOfLastWorldState = time(0);
    static int currentDelayBetweenPacketSend = 0;

        // "Primitive" Connection Functions
    void SetUpInitialListening(int& port, UDPSocketPtr& listeningSocket, SocketAddressPtr& listeningAddress);
    void HandleListening(bool& connectionsOpen, UDPSocketPtr& listeningSocket, SocketAddressPtr& addressRecievedFrom, priority_queue<pair<int, void*>>& unprocessedData);
    
    void SetUpSending(int portToSendTo, int portUsedForSending, UDPSocketPtr& sendingSocket, SocketAddressPtr& sendingAddress);
    
        // Derived Connection Functions for in-game use
    // creator
    bool HandleIncomingInputPackets(priority_queue<pair<int, void*>>& unprocessedData, vector<JoinerInput>& joinerInputs, system_clock::time_point& lastConnectionTime);
    bool HandleOutgoingWorldStatePackets(WorldState& gameWorld, UDPSocketPtr& sendingSocket, SocketAddressPtr& sendingAddress, int delta);
    // joiner
    bool HandleIncomingWorldStatePackets(WorldState& gameWorld, priority_queue<pair<int, void*>>& unprocessedDataPackets, system_clock::time_point& connectionTimer);
    bool HandleOutgoingInputPackets(vector<JoinerInput>& joinerInputs, UDPSocketPtr& sendingSocket, SocketAddressPtr& sendingAddress, system_clock::time_point& lastTimeOfSendingConnection, int delta);
}



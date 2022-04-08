#pragma once

#include "GameFiles/JoinerInput.h"
#include "WorldState.h"

namespace NetworkManager
{
    // can probably be bigger since we're using UDP
    const int BUFFER_SIZE = 4096 * 8;

    const string HOME_ADDRESS = "127.0.0.1:";
    const string ACCEPT_ALL_ADDRESS = "0.0.0.0:";

    static time_t timeOfLastWorldState = time(0);

        // "Primitive" Connection Functions
    void SetUpInitialListening(int& port, UDPSocketPtr& listeningSocket, SocketAddressPtr& listeningAddress);
    void HandleListening(bool& connectionsOpen, UDPSocketPtr& listeningSocket, SocketAddressPtr& addressRecievedFrom, priority_queue<pair<int, void*>>& unprocessedData);
    
    void SetUpSending(int portToSendTo, int portUsedForSending, UDPSocketPtr& sendingSocket, SocketAddressPtr& sendingAddress);
    
        // Derived Connection Functions for in-game use
    // creator
    bool HandleIncomingInputPackets(priority_queue<pair<int, void*>>& unprocessedData, vector<JoinerInput>& joinerInputs);
    bool HandleOutgoingWorldStatePackets(WorldState& gameWorld, UDPSocketPtr& sendingSocket, SocketAddressPtr& sendingAddress);
    // joiner
    bool HandleIncomingWorldStatePackets(WorldState& gameWorld, priority_queue<pair<int, void*>>& unprocessedDataPackets);
    bool HandleOutgoingInputPackets(vector<JoinerInput>& joinerInputs, UDPSocketPtr& sendingSocket, SocketAddressPtr& sendingAddress);
}



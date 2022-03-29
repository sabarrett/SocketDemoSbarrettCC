#pragma once

#include "GameFiles/JoinerInput.h"

namespace NetworkManager
{
    // can probably be bigger since we're using UDP
    const int BUFFER_SIZE = 4096;

    const string HOME_ADDRESS = "127.0.0.1:";
    const string ACCEPT_ALL_ADDRESS = "0.0.0.0:";

        // "Primitive" Connection Functions
    void SetUpInitialListening(int& port, UDPSocketPtr& listeningSocket, SocketAddressPtr& listeningAddress);
    void HandleListening(bool& connectionsOpen, UDPSocketPtr listeningSocket, SocketAddressPtr addressRecievedFrom, vector<std::pair<int, void*>>& unprocessedData);
    
    void SetUpSending(int portToSendTo, int portUsedForSending, UDPSocketPtr sendingSocket, SocketAddressPtr sendingAddress);
    
        // Derived Connection Functions for in-game use
    // creator
    bool HandleIncomingInputPackets(vector<std::pair<int, void*>>& unprocessedData);
    bool HandleOutgoingWorldStatePackets();
    // joiner
    bool HandleIncomingWorldStatePackets();
    bool HandleOutgoingInputPackets(vector<JoinerInput>& joinerInputs, UDPSocketPtr sendingSocket, SocketAddressPtr sendingAddress);
}



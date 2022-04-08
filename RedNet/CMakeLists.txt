cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(RedNet)

find_library(ws Ws2_32.lib)

include_directories("Header Files" "Source Files" "../RoboCat/Inc")
add_library(RedNet "Source Files/TCPNetworkManager.cpp" "../RoboCat/Src/TCPSocket.cpp" "../RoboCat/Src/SocketUtil.cpp" "../RoboCat/Src/SocketAddress.cpp" "../RoboCat/Src/UDPSocket.cpp" "../RoboCat/Src/SocketAddressFactory.cpp" "../RoboCat/Src/StringUtils.cpp")
target_link_libraries(RedNet PUBLIC ${ws})

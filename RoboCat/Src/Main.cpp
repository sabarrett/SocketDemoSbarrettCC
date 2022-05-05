#include "RoboCatPCH.h"

#include "PlayerUser.h"


PlayerUser* user;

std::string CLIENT_SEND_PORT = "1250", CLIENT_RECV_PORT = "2250";


#if _WIN32
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
	
	CLIENT_SEND_PORT = std::to_string(rand() % 1000 + 8999); 
	CLIENT_RECV_PORT = std::to_string(8080);// i think this works??

	// Server Code --------------
	bool isServer = StringUtils::GetCommandLineArg(1) == "server";
	if (isServer)
	{
		user = new PlayerUser(2);
		user->initTcpServer(CLIENT_RECV_PORT);
	}

	// Client Code --------------
	if (!isServer)
	{
		user = new PlayerUser(1);
		user->initTcpClient(CLIENT_SEND_PORT, CLIENT_RECV_PORT);
	}

	SocketUtil::CleanUp();

	return 0;
}


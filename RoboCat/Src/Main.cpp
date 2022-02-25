
#include "RoboCatPCH.h"

#include <thread>

#if _WIN32

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


	UDPSocketPtr srvSock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	UDPSocketPtr cliSock = CreateBoundSocket("127.0.0.1:9000");
	srvSock->SetNonBlockingMode(true);
	{
		SocketAddressPtr srvAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:9001");
		if (srvAddr == nullptr)
		{
			SocketUtil::ReportError("Create Server Socket");
		}
		srvSock->Bind(*srvAddr);

		std::string msg("Hello server!");
		int bytesSent = cliSock->SendTo(msg.c_str(), msg.length(), *srvAddr);
		if (bytesSent <= 0)
		{
			SocketUtil::ReportError("Client SendTo");
		}
		std::cout << "sent " << bytesSent << " bytes\n";
	}
	
	std::thread srvThread([&srvSock]() {
		char buffer[4096];
		SocketAddress fromAddr;
		int bytesReceived = srvSock->ReceiveFrom(buffer, 4096, fromAddr);
		if (bytesReceived <= 0)
		{
			SocketUtil::ReportError("Server ReceiveFrom");
			return;
		}
		std::string msg(buffer, bytesReceived);
		std::cout << "Received message from " << fromAddr.ToString() << ": " << msg << std::endl;
		});

	srvThread.join();

	SocketUtil::CleanUp();

	return 0;
}

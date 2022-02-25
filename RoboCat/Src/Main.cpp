
#include "RoboCatPCH.h"

#include <thread>;

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

	UDPSocketPtr cliSock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	UDPSocketPtr srvSock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);

	SocketAddressPtr srvAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:9001");

	//clientAddr not needed for long, don't keep it around
	{
		SocketAddressPtr cliAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:9000");
		if (cliAddr == nullptr)
		{
			SocketUtil::ReportError("create client socket");
			ExitProcess(1);
		}
		cliSock->Bind(*cliAddr);
	}
	srvSock->Bind(*srvAddr);

	std::string msg("Hello Server");
	int nBytesSent = cliSock->SendTo(msg.c_str(), msg.length(), *srvAddr);
	if (nBytesSent <= 0)
	{
		SocketUtil::ReportError("Client SendTo");
	}
	std::cout << "Sent " << nBytesSent << " bytes\n";
	

	std::thread srvThread([&srvSock]()
	{
		char buffer[4096];
		SocketAddress fromAddr;
		int nBytesRecieved = srvSock->ReceiveFrom(buffer, 4096, fromAddr);
		if (nBytesRecieved <= 0)
		{
			SocketUtil::ReportError("Server RecieveFrom");
			return;
		}
		std::string msg(buffer, nBytesRecieved);
		std::cout << "Recieved message from: " << fromAddr.ToString() << ": " << msg << std::endl;
	});


	srvThread.join();

	SocketUtil::CleanUp();

	return 0;
}

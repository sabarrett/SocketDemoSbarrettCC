
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

void CreateTCPSocketandAddress();

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
	OutputWindow win;


	std::thread t([&win]()
				  {
					  int msgNo = 1;
					  while (true)
					  {
						  std::this_thread::sleep_for(std::chrono::milliseconds(250));
						  std::string msgIn("~~~auto message~~~");
						  std::stringstream ss(msgIn);
						  ss << msgNo;
						  win.Write(ss.str());
						  msgNo++;
					  }
				  });

	while (true)
	{
		std::string input;
		std::getline(std::cin, input);
		win.WriteFromStdin(input);
	}

	SocketUtil::CleanUp();

	return 0;
}

void initTCP()
{
	// Create socket
	TCPSocketPtr socketPTR = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (!socketPTR)
	{
		SocketUtil::ReportError("Error: No socket was created!");
		ExitProcess(1);
	}
	LOG("%s", "Socket was succesfully created");

	// Create Address
	SocketAddressPtr addressPTR = SocketAddressFactory::CreateIPv4FromString("0.0.0.0");
	if (!addressPTR)
	{
		SocketUtil::ReportError("Error: No address was created!");
		ExitProcess(1);
	}
	LOG("%s", "Address was successfully created")
}

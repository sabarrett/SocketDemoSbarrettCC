
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

void term_print(const char* str) {
	//std::cout << str << std::endl;
}

void term_refresh() {

}

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
	

	CHAR_INFO charInfo[5000] = {};
	int char_count = 0;
	int i = 0;
	while (true)
	{
		
		win.HandleEvents();
		win.Draw();
	
	}

	SocketUtil::CleanUp();

	return 0;
}

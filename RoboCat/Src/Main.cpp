
#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"
#include "allegro_wrapper_functions-main/Colour.h"
#include "allegro_wrapper_functions-main/InputSystem.h"

#include <thread>
#include <vector>

using namespace std;

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

class LeftGameObject
{
	private:
		int xPos;
		int	yPos;
		string imageName;
	public:
		LeftGameObject(int x, int y, string fileName) { xPos = x; yPos = y; imageName = fileName; };
		int getX() { return xPos; };
		int getY() { return yPos; };
		void Update() { xPos--; };
		string getImageName() { return imageName; };
};

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

	/*/SocketUtil::StaticInit();


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

		string msg("Hello server!");
		int bytesSent = cliSock->SendTo(msg.c_str(), msg.length(), *srvAddr);
		if (bytesSent <= 0)
		{
			SocketUtil::ReportError("Client SendTo");
		}
		cout << "sent " << bytesSent << " bytes\n";
	}*/

	GraphicsLibrary mLibrary(800, 600);
	mLibrary.init("../2517597.jpg");
	mLibrary.loadImage("../2517597.jpg", "background");
	mLibrary.drawImage("background", 0, 0);
	mLibrary.render();

	InputSystem mInputSystem;
	mInputSystem.init(&mLibrary);

	vector<LeftGameObject*> vLeftGameObjects;


	bool escapePressed = false;

	while (!escapePressed)
	{
		for (LeftGameObject* obj : vLeftGameObjects)
		{
			obj->Update();
			mLibrary.drawImage("LeftObject", obj->getX(), obj->getY());
		}
		int mousePress = mInputSystem.getMouseInput();
		int keyPress = mInputSystem.getKeyboardInput();

		if (keyPress == KeyCode::KeyEscape)
			escapePressed = true;

		switch (mousePress)
		{
			case MouseButton::LeftMouse:
			{
				mLibrary.loadImage("../output-onlinepngtools.png", "LeftObject");
				vLeftGameObjects.push_back(new LeftGameObject((int)mInputSystem.getMouseX(), (int)mInputSystem.getMouseY(), "../output-onlinepngtools.png"));
			}
		}
		mLibrary.render();
		mLibrary.drawImage("background", 0, 0);
	}
	
	//scott recommends no threads

	/*std::thread srvThread([&srvSock]() {
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

	srvThread.join();*/

	SocketUtil::CleanUp();

	return 0;
}

class TCPSocket
{
public:
	~TCPSocket();
	int								Connect(const SocketAddress& inAddress);
	int								Bind(const SocketAddress& inToAddress);
	int								Listen(int inBackLog = 32);
	shared_ptr< TCPSocket >			Accept(SocketAddress& inFromAddress);
	int32_t							Send(const void* inData, size_t inLen);
	int32_t							Receive(void* inBuffer, size_t inLen);
	int								SetNonBlockingMode(bool inShouldBeNonBlocking);
private:
	friend class SocketUtil;
	TCPSocket(SOCKET inSocket) : mSocket(inSocket) {}
	SOCKET		mSocket;
};
// typedef TCPSocket* TCPSocketPtr
// TCPSocketPtr socketPtr2 = sockPtr; // inc ref count
// goes out of scope -> ref count decrement
// ref count == 0 -> delete ptr
typedef shared_ptr< TCPSocket > TCPSocketPtr;
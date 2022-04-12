class Packet;
class TCPSocket
{
public:
	~TCPSocket();
	int								Connect( const SocketAddress& inAddress );
	int								Bind( const SocketAddress& inToAddress );
	int								Listen( int inBackLog = 32 );
	shared_ptr< TCPSocket >			Accept( SocketAddress& inFromAddress );
	int32_t							Send( const void* inData, size_t inLen );
	int32_t							Receive( void* inBuffer, size_t inLen );
	int								SetNonBlockingMode(bool inShouldBeNonBlocking);

	bool HasRead();
	bool HasWrite();
private:
	friend class SocketUtil;
	TCPSocket( SOCKET inSocket ) : mSocket( inSocket ) {}
	SOCKET		mSocket;
	bool mIsNonBlocking = false;
};
typedef shared_ptr< TCPSocket > TCPSocketPtr;

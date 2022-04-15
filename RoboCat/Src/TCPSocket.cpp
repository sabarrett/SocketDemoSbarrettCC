#include "RoboCatPCH.h"


int TCPSocket::Connect( const SocketAddress& inAddress )
{
	int err = connect( mSocket, &inAddress.mSockAddr, inAddress.GetSize() );
	if( err < 0 )
	{
		SocketUtil::ReportError( "TCPSocket::Connect" );
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

int TCPSocket::Listen( int inBackLog )
{
	int err = listen( mSocket, inBackLog );
	if( err < 0 )
	{
		SocketUtil::ReportError( "TCPSocket::Listen" );
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

TCPSocketPtr TCPSocket::Accept( SocketAddress& inFromAddress )
{
	socklen_t length = inFromAddress.GetSize();
	SOCKET newSocket = accept( mSocket, &inFromAddress.mSockAddr, &length );

	if( newSocket != INVALID_SOCKET )
	{
		return TCPSocketPtr( new TCPSocket( newSocket ) );
	}
	else
	{
		SocketUtil::ReportError( "TCPSocket::Accept" );
		return nullptr;
	}
}

int32_t	TCPSocket::Send( const void* inData, size_t inLen )
{
	int bytesSentCount = send( mSocket, static_cast< const char* >( inData ), inLen, 0 );
	if( bytesSentCount < 0 )
	{
		SocketUtil::ReportError( "TCPSocket::Send" );
		return -SocketUtil::GetLastError();
	}
	return bytesSentCount;
}

int32_t	TCPSocket::Receive( void* inData, size_t inLen )
{
	int bytesReceivedCount = recv( mSocket, static_cast< char* >( inData ), inLen, 0 );
	if( bytesReceivedCount < 0 )
	{
		int error = -SocketUtil::GetLastError();
		if (error != -10035)
		{
			SocketUtil::ReportError("TCPSocket::Receive");
		}
		return error;
	}
	return bytesReceivedCount;
}

int TCPSocket::Bind( const SocketAddress& inBindAddress )
{
	int error = bind( mSocket, &inBindAddress.mSockAddr, inBindAddress.GetSize() );
	if( error != 0 )
	{
		SocketUtil::ReportError( "TCPSocket::Bind" );
		return SocketUtil::GetLastError();
	}

	return NO_ERROR;
}

int TCPSocket::SetNonBlockingMode(bool inShouldBeNonBlocking)
{
#if _WIN32
	u_long arg = inShouldBeNonBlocking ? 1 : 0;
	int result = ioctlsocket(mSocket, FIONBIO, &arg);
#else
	int flags = fcntl(mSocket, F_GETFL, 0);
	flags = inShouldBeNonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
	int result = fcntl(mSocket, F_SETFL, flags);
#endif

	if (result == SOCKET_ERROR)
	{
		SocketUtil::ReportError("TCPSocket::SetNonBlockingMode");
		return SocketUtil::GetLastError();
	}
	else
	{
		return NO_ERROR;
	}
}

TCPSocket::~TCPSocket()
{
#if _WIN32
	closesocket( mSocket );
#else
	close( mSocket );
#endif
}

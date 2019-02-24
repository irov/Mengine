#include "PosixSocket.h"

#include "Kernel/FactorableUnique.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Mengine
{
    PosixSocket::PosixSocket()
        : m_socket( INVALID_SOCKET )
        , m_isBlocking( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PosixSocket::~PosixSocket()
    {
        this->disconnect();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixSocket::connect( const SocketConnectInfo & _info )
    {
        addrinfo hints;
        memset( &hints, 0, sizeof( hints ) );

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        addrinfo *addrinfo;
        INT getaddrinfo_result = ::getaddrinfo( _info.ip, _info.port, &hints, &addrinfo );

        if( getaddrinfo_result != 0 )
        {
            return false;
        }

        SOCKET socket = ::socket( addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol );

        if( socket == 0 )
        {
            return false;
        }

        int32_t connect_result = ::connect( socket, addrinfo->ai_addr, (int)addrinfo->ai_addrlen );

        freeaddrinfo( addrinfo );

        if( connect_result != 0 )
        {
            return false;
        }

        m_socket = socket;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixSocket::bind( const SocketConnectInfo & _info, bool _blocking )
    {
        m_socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

        if( !m_socket || INVALID_SOCKET == m_socket )
        {
            return false;
        }

        sockaddr_in addr;
        memset( &addr, 0, sizeof( addr ) );

        const uint16_t port = static_cast<uint16_t>(std::stoi( _info.port ));

        addr.sin_family = AF_INET;
        addr.sin_port = ::htons( port );
        addr.sin_addr.s_addr = ::inet_addr( _info.ip );

        if( ::bind( m_socket, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) == SOCKET_ERROR )
        {
            ::close( m_socket );
            m_socket = INVALID_SOCKET;

            return false;
        }

        m_isBlocking = _blocking;
        u_long arg = _blocking ? 0u : 1u;
        ::fcntl( m_socket, F_SETFL, O_NONBLOCK, &arg );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t PosixSocket::checkForClientConnection()
    {
        if( ::listen( m_socket, SOMAXCONN ) == SOCKET_ERROR )
        {
            ::close( m_socket );
            m_socket = INVALID_SOCKET;

            return -1;
        }

        sockaddr_in clientAddr;
        int32_t addLen = sizeof( clientAddr );
        memset( &clientAddr, 0, addLen );

        SOCKET clientSocket = ::accept( m_socket, reinterpret_cast<sockaddr *>(&clientAddr), &addLen );

        int32_t result = 1;

        if( clientSocket == INVALID_SOCKET )
        {
            if( m_isBlocking == false && errno  == EWOULDBLOCK )
            {
                // no connections yet
                result = 0;
            }
            else
            {
                ::close( m_socket );
                result = -1;
            }
        }
        else
        {
            ::close( m_socket );
            m_socket = clientSocket;
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PosixSocket::waitForData( uint32_t timeoutMs )
    {
        if( m_socket == INVALID_SOCKET )
        {
            return false;
        }

        fd_set socketsSet;
        FD_ZERO(&socketsSet);
        FD_SET(m_socket, &socketsSet);

        timeval tv;
        tv.tv_sec = static_cast<long>(timeoutMs / 1000);
        tv.tv_usec = static_cast<long>((timeoutMs - static_cast<size_t>(tv.tv_sec * 1000)) * 1000);

        int32_t result = ::select( 0, &socketsSet, nullptr, nullptr, (timeoutMs == 0) ? nullptr : &tv );

        return (1 == result);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t PosixSocket::send( const void * _buffer, size_t _size )
    {
        if( m_socket == INVALID_SOCKET )
        {
            return 0;
        }

        int32_t numBytesSent = ::send( m_socket, _buffer, _size, 0 );

        return numBytesSent;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t PosixSocket::receive( void * _buffer, size_t _size )
    {
        if( m_socket == INVALID_SOCKET )
        {
            return 0;
        }

        const int32_t numBytesReceived = ::recv( m_socket, _buffer, _size, 0 );

        return numBytesReceived;
    }
    //////////////////////////////////////////////////////////////////////////
    void PosixSocket::disconnect()
    {
        if( m_socket != INVALID_SOCKET )
        {
            ::close( m_socket );
            m_socket = INVALID_SOCKET;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr PosixSocket::getSendStream() const
    {
        return OutputStreamInterfacePtr(this);
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr PosixSocket::getReceiveStream() const
    {
        return InputStreamInterfacePtr(this);
    }
    //////////////////////////////////////////////////////////////////////////
    SOCKET PosixSocket::getSocket() const
    {
        return m_socket;
    }
}
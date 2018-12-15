#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Win32Socket.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    Win32Socket::Win32Socket()
        : m_socket(INVALID_SOCKET)
        , m_isBlocking( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32Socket::~Win32Socket()
    {
        this->disconnect();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::connect( const SocketConnectInfo & _info )
    {
        addrinfo hints;
        ZeroMemory( &hints, sizeof( hints ) );

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

        int connect_result = ::connect( socket, addrinfo->ai_addr, (int)addrinfo->ai_addrlen );

        freeaddrinfo( addrinfo );

        if( connect_result != 0 )
        {
            return false;
        }

        m_socket = socket;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::bind( const SocketConnectInfo & _info, const bool _blocking)
    {
        m_socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

        if( !m_socket || INVALID_SOCKET == m_socket )
        {
            return false;
        }

        SOCKADDR_IN addr;
        ZeroMemory( &addr, sizeof( addr ) );

        const uint16_t port = static_cast<uint16_t>( std::stoi( _info.port ) );

        addr.sin_family = AF_INET;
        addr.sin_port = ::htons( port );
        addr.sin_addr.s_addr = ::inet_addr( _info.ip );

        if( ::bind( m_socket, reinterpret_cast<LPSOCKADDR>( &addr ), sizeof( addr ) ) == SOCKET_ERROR )
        {
            ::closesocket( m_socket );
            m_socket = INVALID_SOCKET;

            return false;
        }

        m_isBlocking = _blocking;
        u_long arg = _blocking ? 0u : 1u;
        ::ioctlsocket( m_socket, FIONBIO, &arg );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    int Win32Socket::checkForClientConnection()
    {
        if( ::listen( m_socket, SOMAXCONN ) == SOCKET_ERROR )
        {
            ::closesocket( m_socket );
            m_socket = INVALID_SOCKET;

            return -1;
        }

        sockaddr_in clientAddr;
        int addLen = sizeof( clientAddr );
        ZeroMemory( &clientAddr, addLen );

        SOCKET clientSocket = ::accept( m_socket, reinterpret_cast<LPSOCKADDR>( &clientAddr ), &addLen );

        int result = 1;

        if( clientSocket == INVALID_SOCKET )
        {
            const int lastError = ::WSAGetLastError();

            if( m_isBlocking == false && lastError == WSAEWOULDBLOCK )
            {
                // no connections yet
                result = 0;
            }
            else
            {
                ::closesocket( m_socket );
                result = -1;
            }
        }
        else
        {
            ::closesocket( m_socket );
            m_socket = clientSocket;
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::waitForData( size_t timeoutMs )
    {
        if( m_socket == INVALID_SOCKET )
        {
            return false;
        }

        fd_set socketsSet;
        socketsSet.fd_count = 1;
        socketsSet.fd_array[0] = m_socket;

        timeval tv;
        tv.tv_sec = static_cast<long>(timeoutMs / 1000);
        tv.tv_usec = static_cast<long>((timeoutMs - static_cast<size_t>(tv.tv_sec * 1000)) * 1000);

        const int result = ::select( 0, &socketsSet, nullptr, nullptr, (timeoutMs == 0) ? nullptr : &tv );

        return (1 == result);
    }
    //////////////////////////////////////////////////////////////////////////
    int Win32Socket::send( const void * _data, const size_t _numBytes )
    {
        if( m_socket == INVALID_SOCKET )
        {
            return 0;
        }

        const int numBytesSent = ::send( m_socket, reinterpret_cast<const char*>( _data ), static_cast<int>( _numBytes ), 0 );

        return numBytesSent;
    }
    //////////////////////////////////////////////////////////////////////////
    int Win32Socket::receive( void* _data, const size_t _maxBytes )
    {
        if( m_socket == INVALID_SOCKET )
        {
            return 0;
        }

        const int numBytesReceived = ::recv( m_socket, reinterpret_cast<char*>( _data ), static_cast<int>( _maxBytes ), 0 );

        return numBytesReceived;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Socket::disconnect()
    {
        if( m_socket != INVALID_SOCKET )
        {
            ::closesocket( m_socket );
            m_socket = INVALID_SOCKET;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Win32Socket::getSendStream() const
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Win32Socket::getReceiveStream() const
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    SOCKET Win32Socket::getSocket() const
    {
        return m_socket;
    }
}
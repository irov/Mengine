#include "Win32Socket.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    Win32Socket::Win32Socket()
        : m_socket( INVALID_SOCKET )
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
    bool Win32Socket::bind( const SocketConnectInfo & _info )
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::waitForClient()
    {
        if( ::listen( m_socket, SOMAXCONN ) == SOCKET_ERROR )
        {
            ::closesocket( m_socket );
            m_socket = INVALID_SOCKET;

            return false;
        }

        sockaddr_in clientAddr;
        int addLen = sizeof( clientAddr );
        ZeroMemory( &clientAddr, addLen );

        SOCKET clientSocket = ::accept( m_socket, reinterpret_cast<LPSOCKADDR>( &clientAddr ), &addLen );

        ::closesocket( m_socket );

        if( !clientSocket || clientSocket == INVALID_SOCKET )
        {
            return false;
        }

        m_socket = clientSocket;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::send( const Mengine::String & _str )
    {
        if( m_socket == INVALID_SOCKET )
        {
            return false;
        }

        ::send( m_socket, _str.c_str(), static_cast<int>( _str.size() ), 0 );

        return true;
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
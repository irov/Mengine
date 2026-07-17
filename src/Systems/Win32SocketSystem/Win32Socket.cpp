#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Win32Socket.h"

#include "Kernel/AssertionNotImplemented.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32Socket::Win32Socket()
        : m_socket( INVALID_SOCKET )
        , m_blocking( true )
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
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        addrinfo * result = nullptr;
        if( ::getaddrinfo( _info.ip, _info.port, &hints, &result ) != 0 )
        {
            return false;
        }

        for( const addrinfo * current = result; current != nullptr; current = current->ai_next )
        {
            SOCKET socket = ::socket( current->ai_family, current->ai_socktype, current->ai_protocol );
            if( socket == INVALID_SOCKET )
            {
                continue;
            }
            if( ::connect( socket, current->ai_addr, static_cast<int32_t>(current->ai_addrlen) ) == 0 )
            {
                m_socket = socket;
                ::freeaddrinfo( result );
                return true;
            }
            ::closesocket( socket );
        }

        ::freeaddrinfo( result );
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::bind( const SocketConnectInfo & _info, bool _blocking )
    {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;
        addrinfo * result = nullptr;
        const Char * host = _info.ip == nullptr || _info.ip[0] == '\0' ? nullptr : _info.ip;
        if( ::getaddrinfo( host, _info.port, &hints, &result ) != 0 )
        {
            return false;
        }

        for( const addrinfo * current = result; current != nullptr; current = current->ai_next )
        {
            SOCKET socket = ::socket( current->ai_family, current->ai_socktype, current->ai_protocol );
            if( socket == INVALID_SOCKET )
            {
                continue;
            }
            BOOL reuse = TRUE;
            ::setsockopt( socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&reuse), sizeof( reuse ) );
            if( ::bind( socket, current->ai_addr, static_cast<int32_t>(current->ai_addrlen) ) == 0 )
            {
                m_socket = socket;
                m_blocking = _blocking;
                u_long value = _blocking == true ? 0UL : 1UL;
                ::ioctlsocket( m_socket, FIONBIO, &value );
                ::freeaddrinfo( result );
                return true;
            }
            ::closesocket( socket );
        }

        ::freeaddrinfo( result );
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Socket::disconnect()
    {
        if( m_socket != INVALID_SOCKET )
        {
            ::closesocket( m_socket );
        }
        m_socket = INVALID_SOCKET;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t Win32Socket::checkForClientConnection()
    {
        if( m_socket == INVALID_SOCKET || ::listen( m_socket, SOMAXCONN ) == SOCKET_ERROR )
        {
            return -1;
        }
        SOCKET accepted = ::accept( m_socket, nullptr, nullptr );
        if( accepted == INVALID_SOCKET )
        {
            if( m_blocking == false && ::WSAGetLastError() == WSAEWOULDBLOCK )
            {
                return 0;
            }
            return -1;
        }
        ::closesocket( m_socket );
        m_socket = accepted;
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::waitForData( uint32_t _timeoutMs )
    {
        if( m_socket == INVALID_SOCKET )
        {
            return false;
        }
        fd_set readSet;
        FD_ZERO( &readSet );
        FD_SET( m_socket, &readSet );
        timeval timeout{static_cast<long>(_timeoutMs / 1000), static_cast<long>((_timeoutMs % 1000) * 1000)};
        timeval * timeoutPtr = _timeoutMs == 0 ? nullptr : &timeout;
        return ::select( 0, &readSet, nullptr, nullptr, timeoutPtr ) == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::send( const void * _data, size_t _size, size_t * const _sent )
    {
        if( m_socket == INVALID_SOCKET || _sent == nullptr )
        {
            return false;
        }
        int32_t result = ::send( m_socket, reinterpret_cast<const char *>(_data), static_cast<int32_t>(_size), 0 );
        if( result == SOCKET_ERROR )
        {
            return false;
        }
        *_sent = static_cast<size_t>(result);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::receive( void * const _data, size_t _capacity, size_t * const _received )
    {
        if( m_socket == INVALID_SOCKET || _received == nullptr )
        {
            return false;
        }
        int32_t result = ::recv( m_socket, reinterpret_cast<char *>(_data), static_cast<int32_t>(_capacity), 0 );
        if( result == SOCKET_ERROR )
        {
            return false;
        }
        *_received = static_cast<size_t>(result);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Win32Socket::getSendStream() const
    {
        return OutputStreamInterfacePtr( const_cast<Win32Socket *>(this) );
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Win32Socket::getReceiveStream() const
    {
        return InputStreamInterfacePtr( const_cast<Win32Socket *>(this) );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Socket::read( void * const _buffer, size_t _size )
    {
        size_t received = 0;
        return this->receive( _buffer, _size, &received ) == true ? received : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::seek( size_t )
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Socket::rewind()
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::rseek( size_t )
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::skip( size_t )
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Socket::tell() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Socket::size() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::eof() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::time( uint64_t * const ) const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::memory( void ** const, size_t * const )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32Socket::write( const void * _data, size_t _size )
    {
        size_t sent = 0;
        return this->send( _data, _size, &sent ) == true ? sent : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Socket::flush()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}

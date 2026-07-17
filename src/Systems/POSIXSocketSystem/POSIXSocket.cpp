#include "POSIXSocket.h"

#include "Kernel/AssertionNotImplemented.h"

#include "Config/StdErrno.h"

#include <fcntl.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXSocket::POSIXSocket()
        : m_socket( -1 )
        , m_blocking( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXSocket::~POSIXSocket()
    {
        this->disconnect();
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::connect( const SocketConnectInfo & _info )
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
            int socket = ::socket( current->ai_family, current->ai_socktype, current->ai_protocol );
            if( socket < 0 )
            {
                continue;
            }

            if( ::connect( socket, current->ai_addr, static_cast<socklen_t>(current->ai_addrlen) ) == 0 )
            {
                m_socket = socket;
                ::freeaddrinfo( result );
                return true;
            }

            ::close( socket );
        }

        ::freeaddrinfo( result );
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::bind( const SocketConnectInfo & _info, bool _blocking )
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
            int socket = ::socket( current->ai_family, current->ai_socktype, current->ai_protocol );
            if( socket < 0 )
            {
                continue;
            }

            int reuse = 1;
            ::setsockopt( socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ) );
            if( ::bind( socket, current->ai_addr, static_cast<socklen_t>(current->ai_addrlen) ) == 0 )
            {
                m_socket = socket;
                m_blocking = _blocking;
                int flags = ::fcntl( m_socket, F_GETFL, 0 );
                if( flags >= 0 )
                {
                    ::fcntl( m_socket, F_SETFL, _blocking == true ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK) );
                }
                ::freeaddrinfo( result );
                return true;
            }

            ::close( socket );
        }

        ::freeaddrinfo( result );
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXSocket::disconnect()
    {
        if( m_socket >= 0 )
        {
            ::close( m_socket );
            m_socket = -1;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t POSIXSocket::checkForClientConnection()
    {
        if( m_socket < 0 || ::listen( m_socket, SOMAXCONN ) != 0 )
        {
            return -1;
        }

        int accepted = ::accept( m_socket, nullptr, nullptr );
        if( accepted < 0 )
        {
            if( m_blocking == false && (errno == EAGAIN || errno == EWOULDBLOCK) )
            {
                return 0;
            }
            return -1;
        }

        ::close( m_socket );
        m_socket = accepted;
        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::waitForData( uint32_t _timeoutMs )
    {
        if( m_socket < 0 )
        {
            return false;
        }

        fd_set readSet;
        FD_ZERO( &readSet );
        FD_SET( m_socket, &readSet );

        timeval timeout{static_cast<long>(_timeoutMs / 1000), static_cast<int>((_timeoutMs % 1000) * 1000)};
        timeval * timeoutPtr = _timeoutMs == 0 ? nullptr : &timeout;
        return ::select( m_socket + 1, &readSet, nullptr, nullptr, timeoutPtr ) == 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::send( const void * _data, size_t _size, size_t * const _sent )
    {
        if( m_socket < 0 || _sent == nullptr )
        {
            return false;
        }

        ssize_t result = ::send( m_socket, _data, _size, 0 );
        if( result < 0 )
        {
            return false;
        }

        *_sent = static_cast<size_t>(result);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::receive( void * const _data, size_t _capacity, size_t * const _received )
    {
        if( m_socket < 0 || _received == nullptr )
        {
            return false;
        }

        ssize_t result = ::recv( m_socket, _data, _capacity, 0 );
        if( result < 0 )
        {
            return false;
        }

        *_received = static_cast<size_t>(result);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr POSIXSocket::getSendStream() const
    {
        return OutputStreamInterfacePtr( const_cast<POSIXSocket *>(this) );
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr POSIXSocket::getReceiveStream() const
    {
        return InputStreamInterfacePtr( const_cast<POSIXSocket *>(this) );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t POSIXSocket::read( void * const _buffer, size_t _size )
    {
        size_t received = 0;
        return this->receive( _buffer, _size, &received ) == true ? received : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::seek( size_t )
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXSocket::rewind()
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::rseek( size_t )
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::skip( size_t )
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t POSIXSocket::tell() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t POSIXSocket::size() const
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::eof() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::time( uint64_t * const ) const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::memory( void ** const, size_t * const )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t POSIXSocket::write( const void * _data, size_t _size )
    {
        size_t sent = 0;
        return this->send( _data, _size, &sent ) == true ? sent : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocket::flush()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}

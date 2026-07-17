#include "POSIXDatagram.h"

#include "Config/StdErrno.h"
#include "Config/StdString.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool toNativeAddress( const SocketAddress & _address, sockaddr_storage * const _native, socklen_t * const _nativeSize )
        {
            Mengine::StdString::memset( _native, 0, sizeof( sockaddr_storage ) );

            if( _address.family == ESocketAddressFamily::IPv4 )
            {
                sockaddr_in * address = reinterpret_cast<sockaddr_in *>(_native);
                address->sin_family = AF_INET;
                address->sin_port = htons( _address.port );
                Mengine::StdString::memcpy( &address->sin_addr, _address.address, sizeof( address->sin_addr ) );
                *_nativeSize = sizeof( sockaddr_in );
                return true;
            }

            if( _address.family == ESocketAddressFamily::IPv6 )
            {
                sockaddr_in6 * address = reinterpret_cast<sockaddr_in6 *>(_native);
                address->sin6_family = AF_INET6;
                address->sin6_port = htons( _address.port );
                Mengine::StdString::memcpy( &address->sin6_addr, _address.address, sizeof( address->sin6_addr ) );
                *_nativeSize = sizeof( sockaddr_in6 );
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool fromNativeAddress( const sockaddr_storage & _native, socklen_t _nativeSize, SocketAddress * const _address )
        {
            if( _address == nullptr )
            {
                return true;
            }

            *_address = SocketAddress{};
            if( _native.ss_family == AF_INET && _nativeSize >= sizeof( sockaddr_in ) )
            {
                const sockaddr_in * address = reinterpret_cast<const sockaddr_in *>(&_native);
                _address->family = ESocketAddressFamily::IPv4;
                _address->port = ntohs( address->sin_port );
                Mengine::StdString::memcpy( _address->address, &address->sin_addr, sizeof( address->sin_addr ) );
                return true;
            }

            if( _native.ss_family == AF_INET6 && _nativeSize >= sizeof( sockaddr_in6 ) )
            {
                const sockaddr_in6 * address = reinterpret_cast<const sockaddr_in6 *>(&_native);
                _address->family = ESocketAddressFamily::IPv6;
                _address->port = ntohs( address->sin6_port );
                Mengine::StdString::memcpy( _address->address, &address->sin6_addr, sizeof( address->sin6_addr ) );
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXDatagram::POSIXDatagram()
        : m_socket( -1 )
        , m_family( ESocketAddressFamily::Unknown )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXDatagram::~POSIXDatagram()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::open( ESocketAddressFamily _family )
    {
        this->close();
        int nativeFamily = _family == ESocketAddressFamily::IPv6 ? AF_INET6 : (_family == ESocketAddressFamily::IPv4 ? AF_INET : AF_UNSPEC);
        if( nativeFamily == AF_UNSPEC )
        {
            return ESocketResult::AddressError;
        }

        m_socket = ::socket( nativeFamily, SOCK_DGRAM, IPPROTO_UDP );
        if( m_socket < 0 )
        {
            return this->errorResult_();
        }

        m_family = _family;
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::bind( const SocketAddress & _address )
    {
        sockaddr_storage native{};
        socklen_t nativeSize = 0;
        if( m_socket < 0 || Detail::toNativeAddress( _address, &native, &nativeSize ) == false )
        {
            return ESocketResult::AddressError;
        }
        if( ::bind( m_socket, reinterpret_cast<const sockaddr *>(&native), nativeSize ) != 0 )
        {
            return this->errorResult_();
        }
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::connect( const SocketAddress & _address )
    {
        sockaddr_storage native{};
        socklen_t nativeSize = 0;
        if( m_socket < 0 || Detail::toNativeAddress( _address, &native, &nativeSize ) == false )
        {
            return ESocketResult::AddressError;
        }
        if( ::connect( m_socket, reinterpret_cast<const sockaddr *>(&native), nativeSize ) != 0 )
        {
            return this->errorResult_();
        }
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::setBlocking( bool _blocking )
    {
        if( m_socket < 0 )
        {
            return ESocketResult::Failure;
        }
        int flags = ::fcntl( m_socket, F_GETFL, 0 );
        if( flags < 0 || ::fcntl( m_socket, F_SETFL, _blocking == true ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK) ) != 0 )
        {
            return this->errorResult_();
        }
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::setReuseAddress( bool _reuse )
    {
        int value = _reuse == true ? 1 : 0;
        if( m_socket < 0 || ::setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof( value ) ) != 0 )
        {
            return this->errorResult_();
        }
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::setSendBufferSize( uint32_t _size )
    {
        int value = static_cast<int>(_size);
        if( m_socket < 0 || ::setsockopt( m_socket, SOL_SOCKET, SO_SNDBUF, &value, sizeof( value ) ) != 0 )
        {
            return this->errorResult_();
        }
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::setReceiveBufferSize( uint32_t _size )
    {
        int value = static_cast<int>(_size);
        if( m_socket < 0 || ::setsockopt( m_socket, SOL_SOCKET, SO_RCVBUF, &value, sizeof( value ) ) != 0 )
        {
            return this->errorResult_();
        }
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::sendTo( const SocketAddress & _address, const void * _data, size_t _size, size_t * const _sent )
    {
        sockaddr_storage native{};
        socklen_t nativeSize = 0;
        if( m_socket < 0 || _sent == nullptr || Detail::toNativeAddress( _address, &native, &nativeSize ) == false )
        {
            return ESocketResult::AddressError;
        }
        ssize_t result = ::sendto( m_socket, _data, _size, 0, reinterpret_cast<const sockaddr *>(&native), nativeSize );
        if( result < 0 )
        {
            return this->errorResult_();
        }
        *_sent = static_cast<size_t>(result);
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::send( const void * _data, size_t _size, size_t * const _sent )
    {
        if( m_socket < 0 || _sent == nullptr )
        {
            return ESocketResult::Failure;
        }
        ssize_t result = ::send( m_socket, _data, _size, 0 );
        if( result < 0 )
        {
            return this->errorResult_();
        }
        *_sent = static_cast<size_t>(result);
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::receiveFrom( void * const _data, size_t _capacity, size_t * const _received, SocketAddress * const _address )
    {
        if( m_socket < 0 || _received == nullptr )
        {
            return ESocketResult::Failure;
        }

        sockaddr_storage native{};
        iovec vector{_data, _capacity};
        msghdr message{};
        message.msg_name = &native;
        message.msg_namelen = sizeof( native );
        message.msg_iov = &vector;
        message.msg_iovlen = 1;

        ssize_t result = ::recvmsg( m_socket, &message, MSG_TRUNC );
        if( result < 0 )
        {
            return this->errorResult_();
        }
        if( Detail::fromNativeAddress( native, message.msg_namelen, _address ) == false )
        {
            return ESocketResult::AddressError;
        }
        *_received = result > static_cast<ssize_t>(_capacity) ? _capacity : static_cast<size_t>(result);
        return result > static_cast<ssize_t>(_capacity) || (message.msg_flags & MSG_TRUNC) != 0 ? ESocketResult::Truncated : ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::receive( void * const _data, size_t _capacity, size_t * const _received )
    {
        if( m_socket < 0 || _received == nullptr )
        {
            return ESocketResult::Failure;
        }
        ssize_t result = ::recv( m_socket, _data, _capacity, 0 );
        if( result < 0 )
        {
            return this->errorResult_();
        }
        *_received = static_cast<size_t>(result);
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::waitReadable( uint32_t _timeoutMs )
    {
        return this->wait_( true, _timeoutMs );
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::waitWritable( uint32_t _timeoutMs )
    {
        return this->wait_( false, _timeoutMs );
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::getLocalAddress( SocketAddress * const _address ) const
    {
        if( m_socket < 0 || _address == nullptr )
        {
            return ESocketResult::Failure;
        }
        sockaddr_storage native{};
        socklen_t nativeSize = sizeof( native );
        if( ::getsockname( m_socket, reinterpret_cast<sockaddr *>(&native), &nativeSize ) != 0 )
        {
            return ESocketResult::Failure;
        }
        return Detail::fromNativeAddress( native, nativeSize, _address ) == true ? ESocketResult::Successful : ESocketResult::AddressError;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXDatagram::close()
    {
        if( m_socket >= 0 )
        {
            ::close( m_socket );
        }
        m_socket = -1;
        m_family = ESocketAddressFamily::Unknown;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::wait_( bool _read, uint32_t _timeoutMs )
    {
        if( m_socket < 0 )
        {
            return ESocketResult::Failure;
        }
        fd_set set;
        FD_ZERO( &set );
        FD_SET( m_socket, &set );
        timeval timeout{static_cast<long>(_timeoutMs / 1000), static_cast<int>((_timeoutMs % 1000) * 1000)};
        int result = ::select( m_socket + 1, _read == true ? &set : nullptr, _read == true ? nullptr : &set, nullptr, &timeout );
        if( result > 0 )
        {
            return ESocketResult::Successful;
        }
        if( result == 0 )
        {
            return ESocketResult::Timeout;
        }
        return this->errorResult_();
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXDatagram::errorResult_()
    {
        const int32_t error = errno;
        if( error == EAGAIN || error == EWOULDBLOCK )
        {
            return ESocketResult::WouldBlock;
        }
        if( error == ETIMEDOUT )
        {
            return ESocketResult::Timeout;
        }
        if( error == ECONNRESET || error == ECONNREFUSED )
        {
            return ESocketResult::ConnectionReset;
        }
        if( error == EMSGSIZE )
        {
            return ESocketResult::Truncated;
        }
        return ESocketResult::Failure;
    }
    //////////////////////////////////////////////////////////////////////////
}

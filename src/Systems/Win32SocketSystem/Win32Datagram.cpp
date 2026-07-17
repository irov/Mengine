#include "Win32Datagram.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool toNativeAddress( const SocketAddress & _address, sockaddr_storage * const _native, int32_t * const _nativeSize )
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
        static bool fromNativeAddress( const sockaddr_storage & _native, int32_t _nativeSize, SocketAddress * const _address )
        {
            if( _address == nullptr )
            {
                return true;
            }
            *_address = SocketAddress{};
            if( _native.ss_family == AF_INET && _nativeSize >= static_cast<int32_t>(sizeof( sockaddr_in )) )
            {
                const sockaddr_in * address = reinterpret_cast<const sockaddr_in *>(&_native);
                _address->family = ESocketAddressFamily::IPv4;
                _address->port = ntohs( address->sin_port );
                Mengine::StdString::memcpy( _address->address, &address->sin_addr, sizeof( address->sin_addr ) );
                return true;
            }
            if( _native.ss_family == AF_INET6 && _nativeSize >= static_cast<int32_t>(sizeof( sockaddr_in6 )) )
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
    Win32Datagram::Win32Datagram()
        : m_socket( INVALID_SOCKET )
        , m_family( ESocketAddressFamily::Unknown )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32Datagram::~Win32Datagram()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::open( ESocketAddressFamily _family )
    {
        this->close();
        int nativeFamily = _family == ESocketAddressFamily::IPv6 ? AF_INET6 : (_family == ESocketAddressFamily::IPv4 ? AF_INET : AF_UNSPEC);
        if( nativeFamily == AF_UNSPEC )
        {
            return ESocketResult::AddressError;
        }
        m_socket = ::socket( nativeFamily, SOCK_DGRAM, IPPROTO_UDP );
        if( m_socket == INVALID_SOCKET )
        {
            return this->errorResult_();
        }
        m_family = _family;
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::bind( const SocketAddress & _address )
    {
        sockaddr_storage native{};
        int32_t nativeSize = 0;
        if( m_socket == INVALID_SOCKET || Detail::toNativeAddress( _address, &native, &nativeSize ) == false )
        {
            return ESocketResult::AddressError;
        }
        return ::bind( m_socket, reinterpret_cast<const sockaddr *>(&native), nativeSize ) == 0 ? ESocketResult::Successful : this->errorResult_();
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::connect( const SocketAddress & _address )
    {
        sockaddr_storage native{};
        int32_t nativeSize = 0;
        if( m_socket == INVALID_SOCKET || Detail::toNativeAddress( _address, &native, &nativeSize ) == false )
        {
            return ESocketResult::AddressError;
        }
        return ::connect( m_socket, reinterpret_cast<const sockaddr *>(&native), nativeSize ) == 0 ? ESocketResult::Successful : this->errorResult_();
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::setBlocking( bool _blocking )
    {
        u_long value = _blocking == true ? 0UL : 1UL;
        return m_socket != INVALID_SOCKET && ::ioctlsocket( m_socket, FIONBIO, &value ) == 0 ? ESocketResult::Successful : this->errorResult_();
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::setReuseAddress( bool _reuse )
    {
        BOOL value = _reuse == true ? TRUE : FALSE;
        return m_socket != INVALID_SOCKET && ::setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&value), sizeof( value ) ) == 0 ? ESocketResult::Successful : this->errorResult_();
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::setSendBufferSize( uint32_t _size )
    {
        int value = static_cast<int>(_size);
        return m_socket != INVALID_SOCKET && ::setsockopt( m_socket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char *>(&value), sizeof( value ) ) == 0 ? ESocketResult::Successful : this->errorResult_();
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::setReceiveBufferSize( uint32_t _size )
    {
        int value = static_cast<int>(_size);
        return m_socket != INVALID_SOCKET && ::setsockopt( m_socket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char *>(&value), sizeof( value ) ) == 0 ? ESocketResult::Successful : this->errorResult_();
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::sendTo( const SocketAddress & _address, const void * _data, size_t _size, size_t * const _sent )
    {
        sockaddr_storage native{};
        int32_t nativeSize = 0;
        if( m_socket == INVALID_SOCKET || _sent == nullptr || Detail::toNativeAddress( _address, &native, &nativeSize ) == false )
        {
            return ESocketResult::AddressError;
        }
        int32_t result = ::sendto( m_socket, reinterpret_cast<const char *>(_data), static_cast<int32_t>(_size), 0, reinterpret_cast<const sockaddr *>(&native), nativeSize );
        if( result == SOCKET_ERROR )
        {
            return this->errorResult_();
        }
        *_sent = static_cast<size_t>(result);
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::send( const void * _data, size_t _size, size_t * const _sent )
    {
        if( m_socket == INVALID_SOCKET || _sent == nullptr )
        {
            return ESocketResult::Failure;
        }
        int32_t result = ::send( m_socket, reinterpret_cast<const char *>(_data), static_cast<int32_t>(_size), 0 );
        if( result == SOCKET_ERROR )
        {
            return this->errorResult_();
        }
        *_sent = static_cast<size_t>(result);
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::receiveFrom( void * const _data, size_t _capacity, size_t * const _received, SocketAddress * const _address )
    {
        if( m_socket == INVALID_SOCKET || _received == nullptr )
        {
            return ESocketResult::Failure;
        }
        sockaddr_storage native{};
        int32_t nativeSize = sizeof( native );
        int32_t result = ::recvfrom( m_socket, reinterpret_cast<char *>(_data), static_cast<int32_t>(_capacity), 0, reinterpret_cast<sockaddr *>(&native), &nativeSize );
        if( result == SOCKET_ERROR )
        {
            if( ::WSAGetLastError() == WSAEMSGSIZE )
            {
                *_received = _capacity;
                return ESocketResult::Truncated;
            }
            return this->errorResult_();
        }
        if( Detail::fromNativeAddress( native, nativeSize, _address ) == false )
        {
            return ESocketResult::AddressError;
        }
        *_received = static_cast<size_t>(result);
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::receive( void * const _data, size_t _capacity, size_t * const _received )
    {
        if( m_socket == INVALID_SOCKET || _received == nullptr )
        {
            return ESocketResult::Failure;
        }
        int32_t result = ::recv( m_socket, reinterpret_cast<char *>(_data), static_cast<int32_t>(_capacity), 0 );
        if( result == SOCKET_ERROR )
        {
            return this->errorResult_();
        }
        *_received = static_cast<size_t>(result);
        return ESocketResult::Successful;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::waitReadable( uint32_t _timeoutMs )
    {
        return this->wait_( true, _timeoutMs );
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::waitWritable( uint32_t _timeoutMs )
    {
        return this->wait_( false, _timeoutMs );
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::getLocalAddress( SocketAddress * const _address ) const
    {
        if( m_socket == INVALID_SOCKET || _address == nullptr )
        {
            return ESocketResult::Failure;
        }
        sockaddr_storage native{};
        int32_t nativeSize = sizeof( native );
        if( ::getsockname( m_socket, reinterpret_cast<sockaddr *>(&native), &nativeSize ) != 0 )
        {
            return ESocketResult::Failure;
        }
        return Detail::fromNativeAddress( native, nativeSize, _address ) == true ? ESocketResult::Successful : ESocketResult::AddressError;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Datagram::close()
    {
        if( m_socket != INVALID_SOCKET )
        {
            ::closesocket( m_socket );
        }
        m_socket = INVALID_SOCKET;
        m_family = ESocketAddressFamily::Unknown;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32Datagram::wait_( bool _read, uint32_t _timeoutMs )
    {
        if( m_socket == INVALID_SOCKET )
        {
            return ESocketResult::Failure;
        }
        fd_set set;
        FD_ZERO( &set );
        FD_SET( m_socket, &set );
        timeval timeout{static_cast<long>(_timeoutMs / 1000), static_cast<long>((_timeoutMs % 1000) * 1000)};
        int result = ::select( 0, _read == true ? &set : nullptr, _read == true ? nullptr : &set, nullptr, &timeout );
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
    ESocketResult Win32Datagram::errorResult_()
    {
        const int32_t error = ::WSAGetLastError();
        if( error == WSAEWOULDBLOCK )
        {
            return ESocketResult::WouldBlock;
        }
        if( error == WSAETIMEDOUT )
        {
            return ESocketResult::Timeout;
        }
        if( error == WSAECONNRESET || error == WSAECONNREFUSED )
        {
            return ESocketResult::ConnectionReset;
        }
        if( error == WSAEMSGSIZE )
        {
            return ESocketResult::Truncated;
        }
        return ESocketResult::Failure;
    }
    //////////////////////////////////////////////////////////////////////////
}

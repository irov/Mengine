#include "Win32SocketSystem.h"

#include "Win32Socket.h"
#include "Win32Datagram.h"

#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/Logger.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SocketSystem, Mengine::Win32SocketSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool makeSocketAddress( const sockaddr * _native, int32_t _nativeSize, SocketAddress * const _address )
        {
            if( _native == nullptr || _address == nullptr )
            {
                return false;
            }
            *_address = SocketAddress{};

            if( _native->sa_family == AF_INET && _nativeSize >= static_cast<int32_t>(sizeof( sockaddr_in )) )
            {
                const sockaddr_in * address = reinterpret_cast<const sockaddr_in *>(_native);
                _address->family = ESocketAddressFamily::IPv4;
                _address->port = ntohs( address->sin_port );
                Mengine::StdString::memcpy( _address->address, &address->sin_addr, sizeof( address->sin_addr ) );
                return true;
            }

            if( _native->sa_family == AF_INET6 && _nativeSize >= static_cast<int32_t>(sizeof( sockaddr_in6 )) )
            {
                const sockaddr_in6 * address = reinterpret_cast<const sockaddr_in6 *>(_native);
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
    Win32SocketSystem::Win32SocketSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32SocketSystem::~Win32SocketSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SocketSystem::_initializeService()
    {
        int32_t result = ::WSAStartup( MAKEWORD( 2, 2 ), &m_wsaData );
        if( result != 0 )
        {
            LOGGER_ERROR( "invalid WSAStartup [%d]", result );
            return false;
        }

        m_factorySocket = Helper::makeFactoryPoolWithMutex<Win32Socket, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryDatagram = Helper::makeFactoryPoolWithMutex<Win32Datagram, 32>( MENGINE_DOCUMENT_FACTORABLE );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SocketSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDatagram );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySocket );
        m_factoryDatagram = nullptr;
        m_factorySocket = nullptr;
        ::WSACleanup();
    }
    //////////////////////////////////////////////////////////////////////////
    SocketInterfacePtr Win32SocketSystem::createSocket( const DocumentInterfacePtr & _doc )
    {
        Win32SocketPtr socket = m_factorySocket->createObject( _doc );
        MENGINE_ASSERTION_MEMORY_PANIC( socket, "invalid create Win32 socket" );
        return socket;
    }
    //////////////////////////////////////////////////////////////////////////
    DatagramInterfacePtr Win32SocketSystem::createDatagram( const DocumentInterfacePtr & _doc )
    {
        Win32DatagramPtr socket = m_factoryDatagram->createObject( _doc );
        MENGINE_ASSERTION_MEMORY_PANIC( socket, "invalid create Win32 datagram socket" );
        return socket;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult Win32SocketSystem::resolveAddress( const Char * _host, uint16_t _port, ESocketAddressFamily _family, SocketAddress * const _address ) const
    {
        if( _address == nullptr )
        {
            return ESocketResult::AddressError;
        }

        addrinfo hints{};
        hints.ai_family = _family == ESocketAddressFamily::IPv6 ? AF_INET6 : (_family == ESocketAddressFamily::IPv4 ? AF_INET : AF_UNSPEC);
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        hints.ai_flags = _host == nullptr || _host[0] == '\0' ? AI_PASSIVE : 0;

        Char port[8];
        MENGINE_SNPRINTF( port, sizeof( port ), "%u", static_cast<unsigned int>(_port) );
        addrinfo * result = nullptr;
        if( ::getaddrinfo( _host == nullptr || _host[0] == '\0' ? nullptr : _host, port, &hints, &result ) != 0 )
        {
            return ESocketResult::AddressError;
        }

        ESocketResult socketResult = ESocketResult::AddressError;
        for( const addrinfo * current = result; current != nullptr; current = current->ai_next )
        {
            if( Detail::makeSocketAddress( current->ai_addr, static_cast<int32_t>(current->ai_addrlen), _address ) == true )
            {
                socketResult = ESocketResult::Successful;
                break;
            }
        }

        ::freeaddrinfo( result );
        return socketResult;
    }
    //////////////////////////////////////////////////////////////////////////
}

#include "POSIXSocketSystem.h"

#include "POSIXSocket.h"
#include "POSIXDatagram.h"

#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FactoryPool.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SocketSystem, Mengine::POSIXSocketSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool makeSocketAddress( const sockaddr * _native, socklen_t _nativeSize, SocketAddress * const _address )
        {
            if( _native == nullptr || _address == nullptr )
            {
                return false;
            }

            *_address = SocketAddress{};

            if( _native->sa_family == AF_INET && _nativeSize >= sizeof( sockaddr_in ) )
            {
                const sockaddr_in * address = reinterpret_cast<const sockaddr_in *>(_native);
                _address->family = ESocketAddressFamily::IPv4;
                _address->port = ntohs( address->sin_port );
                Mengine::StdString::memcpy( _address->address, &address->sin_addr, sizeof( address->sin_addr ) );
                return true;
            }

            if( _native->sa_family == AF_INET6 && _nativeSize >= sizeof( sockaddr_in6 ) )
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
    POSIXSocketSystem::POSIXSocketSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXSocketSystem::~POSIXSocketSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXSocketSystem::_initializeService()
    {
        m_factorySocket = Helper::makeFactoryPoolWithMutex<POSIXSocket, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryDatagram = Helper::makeFactoryPoolWithMutex<POSIXDatagram, 32>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXSocketSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDatagram );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySocket );

        m_factoryDatagram = nullptr;
        m_factorySocket = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    SocketInterfacePtr POSIXSocketSystem::createSocket( const DocumentInterfacePtr & _doc )
    {
        POSIXSocketPtr socket = m_factorySocket->createObject( _doc );
        MENGINE_ASSERTION_MEMORY_PANIC( socket, "invalid create POSIX socket" );
        return socket;
    }
    //////////////////////////////////////////////////////////////////////////
    DatagramInterfacePtr POSIXSocketSystem::createDatagram( const DocumentInterfacePtr & _doc )
    {
        POSIXDatagramPtr socket = m_factoryDatagram->createObject( _doc );
        MENGINE_ASSERTION_MEMORY_PANIC( socket, "invalid create POSIX datagram socket" );
        return socket;
    }
    //////////////////////////////////////////////////////////////////////////
    ESocketResult POSIXSocketSystem::resolveAddress( const Char * _host, uint16_t _port, ESocketAddressFamily _family, SocketAddress * const _address ) const
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
            if( Detail::makeSocketAddress( current->ai_addr, static_cast<socklen_t>(current->ai_addrlen), _address ) == true )
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

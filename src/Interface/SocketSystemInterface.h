#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/SocketInterface.h"
#include "Interface/DatagramInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SocketSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SocketSystem" )

    public:
        virtual SocketInterfacePtr createSocket( const DocumentInterfacePtr & _doc ) = 0;
        virtual DatagramInterfacePtr createDatagram( const DocumentInterfacePtr & _doc ) = 0;
        virtual ESocketResult resolveAddress( const Char * _host, uint16_t _port, ESocketAddressFamily _family, SocketAddress * const _address ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define SOCKET_SYSTEM()\
    ((Mengine::SocketSystemInterface *)SERVICE_GET(Mengine::SocketSystemInterface))
//////////////////////////////////////////////////////////////////////////

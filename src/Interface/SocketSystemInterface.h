#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/SocketInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SocketSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SocketSystem" )

    public:
        virtual SocketInterfacePtr createSocket() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SOCKET_SYSTEM()\
    ((Mengine::SocketSystemInterface*)SERVICE_GET(Mengine::SocketSystemInterface))
//////////////////////////////////////////////////////////////////////////

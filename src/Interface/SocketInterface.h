#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/StreamInterface.h"

#include "Kernel/Factorable.h"

#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SocketInterface
        : public Factorable
    {
    public:
        virtual bool connect( const Char * _ip, const Char * _port ) = 0;
        virtual void disconnect() = 0;

    public:		
		virtual const OutputStreamInterfacePtr & getSendStream() const = 0;
		virtual const InputStreamInterfacePtr & getReceiveStream() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SocketInterface> SocketInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SocketSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SocketSystem" )

    public:
        virtual SocketInterfacePtr createSocket() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define SOCKET_SYSTEM( serviceProvider )\
    ((Mengine::SocketSystemInterface*)SERVICE_GET(serviceProvider, Mengine::SocketSystemInterface))
}
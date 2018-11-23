#pragma once

#include "Interface/OutputStreamInterface.h"
#include "Interface/InputStreamInterface.h"

#include "Kernel/Mixin.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct SocketConnectInfo
    {
        const Char * ip;
        const Char * port;
    };
    //////////////////////////////////////////////////////////////////////////
    class SocketInterface
        : public Mixin
    {
    public:
        virtual bool connect( const SocketConnectInfo & _data ) = 0;
        virtual void disconnect() = 0;

    public:
        virtual OutputStreamInterfacePtr getSendStream() const = 0;
        virtual InputStreamInterfacePtr getReceiveStream() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SocketInterface> SocketInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
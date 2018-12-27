#pragma once

#include "Interface/OutputStreamInterface.h"
#include "Interface/InputStreamInterface.h"

#include "Kernel/Mixin.h"

#include "Config/Char.h"
#include "Config/String.h"

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
        virtual bool bind( const SocketConnectInfo & _data, const bool _blocking = true ) = 0;
        virtual void disconnect() = 0;

        virtual int32_t checkForClientConnection() = 0;
        virtual bool waitForData( size_t _timeoutMs ) = 0;

        virtual int32_t send( const void* _data, size_t _numBytes ) = 0;
        virtual int32_t receive( void* _data, size_t _maxBytes ) = 0;

    public:
        virtual OutputStreamInterfacePtr getSendStream() const = 0;
        virtual InputStreamInterfacePtr getReceiveStream() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SocketInterface> SocketInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
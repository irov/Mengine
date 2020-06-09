#pragma once

#include "Interface/Interface.h"
#include "Interface/OutputStreamInterface.h"
#include "Interface/InputStreamInterface.h"

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
        : public Interface
    {
    public:
        virtual bool connect( const SocketConnectInfo & _data ) = 0;
        virtual bool bind( const SocketConnectInfo & _data, bool _blocking = true ) = 0;
        virtual void disconnect() = 0;

        virtual int32_t checkForClientConnection() = 0;
        virtual bool waitForData( uint32_t _timeoutMs ) = 0;

        virtual int32_t send( const void * _buffer, size_t _size ) = 0;
        virtual int32_t receive( void * const _buffer, size_t _size ) = 0;

    public:
        virtual OutputStreamInterfacePtr getSendStream() const = 0;
        virtual InputStreamInterfacePtr getReceiveStream() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SocketInterface> SocketInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
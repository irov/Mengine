#pragma once

#include "Interface/ServantInterface.h"
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
        : public ServantInterface
    {
    public:
        virtual bool connect( const SocketConnectInfo & _data ) = 0;
        virtual bool bind( const SocketConnectInfo & _data, bool _blocking = true ) = 0;
        virtual void disconnect() = 0;

        virtual int32_t checkForClientConnection() = 0;
        virtual bool waitForData( uint32_t _timeoutMs ) = 0;

    public:
        virtual bool send( const void * _data, size_t _size, size_t * const _sent ) = 0;
        virtual bool receive( void * const _buffer, size_t _capacity, size_t * const _received ) = 0;

    public:
        virtual OutputStreamInterfacePtr getSendStream() const = 0;
        virtual InputStreamInterfacePtr getReceiveStream() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SocketInterface> SocketInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
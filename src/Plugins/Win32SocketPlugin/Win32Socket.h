#pragma once

#include "Interface/SocketInterface.h"

#include "Win32SocketInputStream.h"
#include "Win32SocketOutputStream.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32Socket
        : public SocketInterface
        , public Win32SocketInputStream
        , public Win32SocketOutputStream
    {
        DECLARE_FACTORABLE( Win32Socket );

    public:
        Win32Socket();
        ~Win32Socket() override;

    public:
        bool connect( const SocketConnectInfo & _info ) override;
        bool bind( const SocketConnectInfo & _info, bool _blocking = true ) override;
        void disconnect() override;

        int32_t checkForClientConnection() override;
        bool waitForData( uint32_t _timeoutMs ) override;

    public:
        bool send( const void * _data, size_t _size, size_t * const _sent ) override;
        bool receive( void * const _data, size_t _capacity, size_t * const _received ) override;

    public:
        OutputStreamInterfacePtr getSendStream() const override;
        InputStreamInterfacePtr getReceiveStream() const override;

    protected:
        SOCKET getSocket() const override;

    protected:
        SOCKET m_socket;
        bool m_isBlocking;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32Socket> Win32SocketPtr;
    //////////////////////////////////////////////////////////////////////////
}
#pragma once

#include "Interface/SocketInterface.h"

#include "PosixSocketInputStream.h"
#include "PosixSocketOutputStream.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class PosixSocket
        : public SocketInterface
        , public PosixSocketInputStream
        , public PosixSocketOutputStream
        , public Factorable
    {
    public:
        PosixSocket();
        ~PosixSocket() override;

    public:
        bool connect( const SocketConnectInfo & _info ) override;
        bool bind( const SocketConnectInfo & _info, bool _blocking = true ) override;
        void disconnect() override;

        int checkForClientConnection() override;
        bool waitForData( uint32_t _timeoutMs ) override;

        int send( const void * _buffer, size_t _size ) override;
        int receive( void * _buffer, size_t _size ) override;

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
    typedef IntrusivePtr<PosixSocket> PosixSocketPtr;
    //////////////////////////////////////////////////////////////////////////
}
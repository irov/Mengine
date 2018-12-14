#pragma once

#include "Interface/SocketInterface.h"

#include "Win32SocketInputStream.h"
#include "Win32SocketOutputStream.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"


namespace Mengine
{
    class Win32Socket
        : public SocketInterface
        , public Win32SocketInputStream
        , public Win32SocketOutputStream
        , public Factorable
    {
    public:
        Win32Socket();
        ~Win32Socket() override;

    public:
        bool connect( const SocketConnectInfo & _info ) override;
        bool bind( const SocketConnectInfo & _info ) override;
        void disconnect() override;

        bool waitForClient() override;
        bool waitForData( size_t _timeoutMs ) override;

        int send( const void* _data, const size_t _numBytes ) override;
        int receive( void* _data, const size_t _maxBytes ) override;

    public:
        OutputStreamInterfacePtr getSendStream() const override;
        InputStreamInterfacePtr getReceiveStream() const override;

    protected:
        SOCKET getSocket() const override;

    protected:
        SOCKET m_socket;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32Socket> Win32SocketPtr;
    //////////////////////////////////////////////////////////////////////////
}
#pragma once

#include "Interface/SocketInterface.h"

#include "Win32SocketInputStream.h"
#include "Win32SocketOutputStream.h"

#include "Environment/WIN32/WindowsIncluder.h"

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
        bool connect( const Char * _ip, const Char * _port ) override;
        void disconnect() override;

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
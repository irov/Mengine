#pragma once

#include "Interface/SocketInterface.h"

#include "Win32SocketInputStream.h"
#include "Win32SocketOutputStream.h"

#include "Environment/WIN32/WindowsIncluder.h"

#pragma warning(push, 0)
#   include <winsock2.h>
#   include <ws2tcpip.h>
#pragma warning(pop)

namespace Mengine
{
    class Win32Socket
        : public SocketInterface
    {
    public:
        Win32Socket();
        ~Win32Socket() override;

    public:
        bool connect( const Char * _ip, const Char * _port ) override;
        void disconnect() override;

    public:
		const OutputStreamInterfacePtr & getSendStream() const override;
		const InputStreamInterfacePtr & getReceiveStream() const override;

    protected:
        SOCKET m_socket;

		Win32SocketInputStreamPtr m_receiveStream;
		Win32SocketOutputStreamPtr m_sendStream;
    };
}
#pragma once

#include "Kernel/Mixin.h"

#include "Environment/Windows/WindowsIncluder.h"

#pragma warning(push, 0)
#   include <winsock2.h>
#   include <ws2tcpip.h>
#pragma warning(pop)

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32SocketProviderInterface
        : public Mixin
    {
    public:
        virtual SOCKET getSocket() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32SocketProviderInterface> Win32SocketProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
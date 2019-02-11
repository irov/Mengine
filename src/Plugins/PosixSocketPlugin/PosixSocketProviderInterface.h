#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    typedef int32_t SOCKET;
    typedef int32_t INT;
    static const SOCKET INVALID_SOCKET = -1;
    static const int32_t SOCKET_ERROR = -1;

    class PosixSocketProviderInterface
        : public Mixin
    {
    public:
        virtual SOCKET getSocket() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PosixSocketProviderInterface> PosixSocketProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
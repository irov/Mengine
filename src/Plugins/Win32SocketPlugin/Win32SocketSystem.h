#pragma once

#include "Interface/SocketSystemInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

#include <winsock2.h> 

namespace Mengine
{
    class Win32SocketSystem
        : public ServiceBase<SocketSystemInterface>
    {
    public:
        Win32SocketSystem();
        ~Win32SocketSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        SocketInterfacePtr createSocket() override;

    protected:
        WSADATA m_wsaData;

        FactoryPtr m_poolWin32Socket;
    };
}
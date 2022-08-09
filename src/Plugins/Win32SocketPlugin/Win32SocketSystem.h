#pragma once

#include "Interface/SocketSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ServiceBase.h"

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
        SocketInterfacePtr createSocket( const DocumentPtr & _doc ) override;

    protected:
        WSADATA m_wsaData;

        FactoryInterfacePtr m_factoryWin32Socket;
    };
}
#pragma once

#include "Interface/SocketSystemInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class PosixSocketSystem
        : public ServiceBase<SocketSystemInterface>
    {
    public:
        PosixSocketSystem();
        ~PosixSocketSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        SocketInterfacePtr createSocket( const Char * _doc ) override;

    protected:
        FactoryPtr m_factorySocket;
    };
}
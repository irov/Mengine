#pragma once

#include "Interface/SocketSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class MockupSocketSystem
        : public ServiceBase<SocketSystemInterface>
    {
    public:
        MockupSocketSystem();
        ~MockupSocketSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        SocketInterfacePtr createSocket( const DocumentInterfacePtr & _doc ) override;
    };
}

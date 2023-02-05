#pragma once

#include "Interface/SocketSystemInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class SDLSocketSystem
        : public ServiceBase<SocketSystemInterface>
    {
    public:
        SDLSocketSystem();
        ~SDLSocketSystem();

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        SocketInterfacePtr createSocket( const DocumentPtr & _doc ) override;

    protected:
        FactoryPtr m_factorySocket;
    };
}

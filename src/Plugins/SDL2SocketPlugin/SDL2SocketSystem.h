#pragma once

#include "Interface/SocketSystemInterface.h"

#include "Kernel/ServiceBase.h"

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
        SocketInterfacePtr createSocket( const DocumentInterfacePtr & _doc ) override;

    protected:
        FactoryInterfacePtr m_factorySocket;
    };
}

#   pragma once

#   include "Interface/SocketInterface.h"

#   include "Core/ServiceBase.h"

#   include "Factory/Factory.h"

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
        SocketInterfacePtr createSocket() override;

    protected:
        FactoryPtr m_factorySocket;
    };
}

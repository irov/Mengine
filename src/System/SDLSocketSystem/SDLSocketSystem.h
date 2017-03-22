#   pragma once

#   include "Interface/SocketInterface.h"

#   include "Core/ServiceBase.h"

#   include "Factory/Factory.h"

namespace Menge
{
    class SDLSocketSystem
        : public ServiceBase<SocketSystemInterface>
    {
    public:
        SDLSocketSystem();
        ~SDLSocketSystem();

    public:
        bool _initialize() override;
        void _finalize() override;

    protected:
        SocketInterfacePtr createSocket() override;

    protected:
        FactoryPtr m_poolSDLSocket;
    };
}

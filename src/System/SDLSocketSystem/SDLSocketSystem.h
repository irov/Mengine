#   pragma once

#   include "Interface/SocketInterface.h"

#   include "Factory/FactoryStore.h"

#   include "SDLSocket.h"


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
        typedef FactoryPoolStore<SDLSocket, 16> TPoolWin32Socket;
        TPoolWin32Socket m_poolSDLSocket;
    };
}

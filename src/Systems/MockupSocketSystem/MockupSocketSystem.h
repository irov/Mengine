#   pragma once

#   include "Interface/SocketInterface.h"

#   include "Core/ServiceBase.h"

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
        SocketInterfacePtr createSocket() override;
	};
}

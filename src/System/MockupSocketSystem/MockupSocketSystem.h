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
        bool _initialize() override;
        void _finalize() override;

    protected:
        SocketInterfacePtr createSocket() override;
	};
}

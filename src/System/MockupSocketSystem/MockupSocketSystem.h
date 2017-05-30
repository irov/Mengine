#   pragma once

#   include "Interface/SocketInterface.h"

#   include "Core/ServiceBase.h"

namespace Menge
{
    class MockupSocketSystem
        : public ServiceBase<SocketSystemInterface>
    {
    public:
		MockupSocketSystem();
        ~MockupSocketSystem();

    public:
        bool _initialize() override;
        void _finalize() override;

    protected:
        SocketInterfacePtr createSocket() override;
	};
}

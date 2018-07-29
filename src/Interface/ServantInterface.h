#pragma once

#include "Config/Typedef.h"

#include "Kernel/Factorable.h"

#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ServantInterface
        : public Factorable
    {
    public:
        ServantInterface() noexcept
        {
        };

        ~ServantInterface() override
        {
        };
    };
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ServantInterface> ServantInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
}
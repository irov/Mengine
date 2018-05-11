#pragma once

#include "Config/Typedef.h"

#include "Factory/Factorable.h"

#include "Core/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ServantInterface
        : public Factorable
    {
    public:
        ServantInterface()
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
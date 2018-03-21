#pragma once

#include "Config/Typedef.h"

#include "Factory/FactorablePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ServantInterface
        : public FactorablePtr
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
	typedef stdex::intrusive_ptr<ServantInterface> ServantInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
}
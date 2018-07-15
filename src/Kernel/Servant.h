#pragma once

#include "Kernel/Factorable.h"

#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	class Servant
		: public Factorable
	{
	public:
		Servant();
		~Servant() override;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Servant> ServantPtr;
    //////////////////////////////////////////////////////////////////////////
}
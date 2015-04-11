#	pragma once

#	include "Core/Memory.h"

namespace Menge
{
	template<class T>
	class FactorableUnique
		: public T
		, public MemoryAllocator
	{
	public:
		FactorableUnique()
		{
		}

    public:
        void destroy() override
        {
			delete this;
        }
	};
}

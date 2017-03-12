#	pragma once

#	include "Core/MemoryAllocator.h"

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
			this->_destroy();

			delete this;
        }
	};
}

#pragma once

#include "Core/MemoryAllocator.h"

namespace Mengine
{
	template<class T>
	class FactorableUnique
		: public T
		, public MemoryAllocator<T>
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

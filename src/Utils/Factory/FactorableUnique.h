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
        template<class ... Args>
		FactorableUnique( Args ... args )
            : T( args ... )
		{
		}

        ~FactorableUnique() override
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

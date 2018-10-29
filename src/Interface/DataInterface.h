#pragma once

#include "Config/Typedef.h"

#include "Kernel/Mixin.h"
#include "Kernel/Pointer.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class DataInterface
		: public Mixin
	{
	public:
		virtual Pointer allocateMemory(size_t _size) const = 0;

	public:
		template<class T>
		inline Pointer allocateMemoryT(size_t _count) const
		{
			size_t total_size = sizeof(T) * _count;

			return this->allocateMemory(total_size);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<DataInterface> DataInterfacePtr;
	//////////////////////////////////////////////////////////////////////////    
}

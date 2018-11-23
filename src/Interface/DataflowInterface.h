#pragma once

#include "Config/Typedef.h"

#include "Interface/DataInterface.h"
#include "Interface/InputStreamInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class DataflowInterface
		: public Mixin
	{
	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual DataInterfacePtr create() = 0;

    public:
		virtual bool load(const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<DataflowInterface> DataflowInterfacePtr;
	//////////////////////////////////////////////////////////////////////////	
}



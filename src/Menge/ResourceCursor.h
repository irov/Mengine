#pragma once

#include "Kernel/ResourceReference.h"

#include "Interface/MemoryInterface.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceCursor
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		virtual const FilePath & getPath() const = 0;
		virtual const MemoryInterfacePtr & getBuffer() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceCursor> ResourceCursorPtr;
}
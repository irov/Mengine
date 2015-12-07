#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "Interface/MemoryInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceCursor
		: public ResourceReference
	{
	public:
		virtual const FilePath & getPath() const = 0;
		virtual const MemoryInterfacePtr & getBuffer() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceCursor> ResourceCursorPtr;
}
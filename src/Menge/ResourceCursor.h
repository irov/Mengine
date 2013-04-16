#	pragma once

#	include "Kernel/ResourceReference.h"

namespace Menge
{
	class ResourceCursor
		: public ResourceReference
	{
	public:
		virtual const FilePath & getPath() const = 0;
		virtual void * getBuffer( size_t & _size ) const = 0;
	};
}
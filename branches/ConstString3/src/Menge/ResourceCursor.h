#	pragma once

#	include "ResourceReference.h"

namespace Menge
{
	class ResourceCursor
		: public ResourceReference
	{
	public:
		virtual const WString& getPath() const = 0;
		virtual void * getBuffer( size_t & _size ) const = 0;
	};
}
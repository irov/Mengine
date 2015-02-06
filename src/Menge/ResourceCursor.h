#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "Config/Blobject.h"

namespace Menge
{
	class ResourceCursor
		: public ResourceReference
	{
	public:
		virtual const FilePath & getPath() const = 0;
		virtual const Blobject & getBuffer() const = 0;
	};
}
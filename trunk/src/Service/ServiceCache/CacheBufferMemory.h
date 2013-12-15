#	pragma once

namespace Menge
{
	struct CacheBufferMemory
	{
		TBlobject::value_type * buffer;
		size_t size;
		bool lock;
	};
}
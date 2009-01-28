#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	class DataStream 
		: public DataStreamInterface
	{
	public:
		virtual bool isMemory() const = 0;
	};
}

#	pragma once

#   include "Interface/StreamInterface.h"

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

namespace Menge
{
	namespace Utils
	{
		void skip( InputStreamInterface * _stream, size_t _count );
		bool eof( InputStreamInterface * _stream );
	}
};

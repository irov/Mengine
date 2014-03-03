#	pragma once

#   include "Interface/StreamInterface.h"

#	include "Config/Typedef.h"

namespace Menge
{
	namespace Utils
	{
		void skip( const InputStreamInterfacePtr & _stream, size_t _count );
		bool eof( const InputStreamInterfacePtr & _stream );
	}
};

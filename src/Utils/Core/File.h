#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

namespace Menge
{
	class InputStreamInterface;
	class OutputStreamInterface;

	namespace Utils
	{
		void skip( InputStreamInterface * _stream, int _count );
		bool eof( InputStreamInterface * _stream );
	}
};

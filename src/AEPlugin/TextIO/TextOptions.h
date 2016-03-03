#	pragma once

#	include "AEIO.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	define TEXT_IO_FLATTEN_OPTIONS_SIZE 2048
	//////////////////////////////////////////////////////////////////////////
	class TextOptions
	{
	public:
		bool readFlatten( const unsigned char * _flatten );
		bool writeFlatten( unsigned char * _flatten );

	public:
		A_Time duration;

		uint32_t width;
		uint32_t height;
	};
}
#	include "ImageDecoderMemory.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderMemory::ImageDecoderMemory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderMemory::decode( void * _buffer, size_t _bufferSize )
	{
		size_t read_byte = m_stream->read( _buffer, _bufferSize );

		return read_byte;
	}
}

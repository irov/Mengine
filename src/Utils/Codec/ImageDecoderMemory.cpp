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
		m_stream->seek( 0 );
		
		size_t read_byte = 0;		
		unsigned char * buffer_ptr = static_cast<unsigned char *>(_buffer);
		for( size_t j = 0; j != m_dataInfo.height; ++j )
		{
			read_byte += m_stream->read( buffer_ptr, m_dataInfo.width * m_dataInfo.channels );

			buffer_ptr += m_options.pitch;
		}		

		return read_byte;
	}
}

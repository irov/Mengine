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
		if( _bufferSize < m_options.pitch * m_dataInfo.height )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderMemory::decode invalid bufferSize %d != (%d * %d)"
				, _bufferSize
				, m_options.pitch
				, m_dataInfo.height
				);

			return 0;
		}

		m_stream->seek( 0 );

		size_t read_byte = 0;
		
		if( m_dataInfo.channels == 3 && m_options.channels == 4 )
		{
			read_byte = this->decodeData_( _buffer );

			this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, _buffer, m_options.pitch );
		}
		else if( m_dataInfo.channels == 4 && m_options.channels == 4 )
		{
			read_byte = this->decodeData_( _buffer );
		}

		return read_byte;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderMemory::decodeData_( void * _buffer ) const
	{
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

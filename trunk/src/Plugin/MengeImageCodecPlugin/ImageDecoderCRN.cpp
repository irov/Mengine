#	include "ImageDecoderCRN.h"

#	include "Core/CacheMemoryBuffer.h"

#	include "Logger/Logger.h"

#	include "inc/crn_decomp_ext.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderCRN::ImageDecoderCRN()
		: m_data_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderCRN::~ImageDecoderCRN()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderCRN::_initialize()
	{		
		unsigned char header[64];
        m_stream->read( header, 64 );
        
		if( crnex::valid_crn_header( header ) == false )
		{
			return false;
		}

		uint32_t faces = crnex::get_crn_header_faces( header );
		if( faces != 1 )
		{
			LOGGER_WARNING(m_serviceProvider)("ImageDecoderCRN::initialize support only 1 faces" 
                );                        

			return false;				 
		}

		m_dataInfo.depth = 1;
		m_dataInfo.mipmaps = 1;
		m_dataInfo.width = crnex::get_crn_header_width( header );
		m_dataInfo.height = crnex::get_crn_header_height( header );

		if( crnex::is_crn_header_format_dxt5( header ) == true )
		{
			m_dataInfo.channels = 4;
			m_dataInfo.format = PF_DXT5;
		}
		else if( crnex::is_crn_header_format_dxt1( header ) == true )
		{
			m_dataInfo.channels = 3;
			m_dataInfo.format = PF_DXT1;
		}
		else
		{
			return false;
		}

		m_dataInfo.flags = 0;      
		m_dataInfo.size = Helper::getTextureMemorySize( m_dataInfo.width, m_dataInfo.height, 1, m_dataInfo.format );

		m_data_size = crnex::get_crn_header_data_size( header );
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderCRN::decode( void * _buffer, size_t _bufferSize )
	{
        (void)_bufferSize;

		CacheMemoryBuffer dxt_buffer(m_serviceProvider, m_data_size, "ImageDecoderCRN");
		void * row_memory = dxt_buffer.getMemory();

		m_stream->read( row_memory, m_data_size );

		if( crnex::decode_crn( _buffer, _bufferSize, row_memory, m_data_size, m_options.pitch ) == false )
		{
			return 0;
		}

		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
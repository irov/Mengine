#	include "ImageDecoderACF.h"
#   include "ImageVerifyACF.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Core/CacheMemoryBuffer.h"

#   include "Config/Blobject.h"

#	include "stdex/memorycopy.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderACF::ImageDecoderACF()
		: m_uncompress_size(0)
		, m_compress_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderACF::~ImageDecoderACF()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderACF::_initialize()
	{		
		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "lz4") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderACF::_prepareData()
	{
        uint32_t magic;
        m_stream->read( &magic, sizeof(magic) );

        if( magic != GET_MAGIC_NUMBER(MAGIC_ACF) )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::initialize invalid htf file magic number" 
                );

            return false;
        }

		uint32_t version;
		m_stream->read( &version, sizeof(version) );

		if( version != GET_MAGIC_VERSION(MAGIC_ACF) )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::initialize invalid htf file magic version" 
				);

			return false;
		}

		uint32_t width;
        m_stream->read( &width, sizeof(width) );

		uint32_t height;
		m_stream->read( &height, sizeof(height) );

		m_stream->read( &m_uncompress_size, sizeof(m_uncompress_size) );
		m_stream->read( &m_compress_size, sizeof(m_compress_size) );

		m_dataInfo.width = width;
		m_dataInfo.height = height;
        m_dataInfo.channels = 1;
		m_dataInfo.format = PF_A8;
		
		m_dataInfo.size = Helper::getTextureMemorySize( m_dataInfo.width, m_dataInfo.height, m_dataInfo.channels, m_dataInfo.depth, m_dataInfo.format );
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderACF::decode( void * _buffer, size_t _bufferSize )
	{
		(void)_bufferSize;
		//if( _bufferSize < m_options.pitch * m_dataInfo.height )
		//{
		//	LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::decode invalid bufferSize %d != (%d * %d)"
		//		, _bufferSize
		//		, m_options.pitch
		//		, m_dataInfo.height
		//		);

		//	return 0;
		//}

		size_t uncompress_size;
		if( m_options.pitch * m_dataInfo.height == m_uncompress_size )
		{			
			if( ARCHIVE_SERVICE(m_serviceProvider)
				->decompressStream( m_archivator, m_stream, m_compress_size, _buffer, m_uncompress_size, uncompress_size ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::decode uncompress failed"
					);

				return 0;
			}		
		}
		else
		{
			CacheMemoryBuffer buffer(m_serviceProvider, m_uncompress_size, "ImageDecoderACF::decode");
			void * memory = buffer.getMemory();

			if( ARCHIVE_SERVICE(m_serviceProvider)
				->decompressStream( m_archivator, m_stream, m_compress_size, memory, m_uncompress_size, uncompress_size ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::decode uncompress failed"
					);

				return 0;
			}	

			const unsigned char * source_buffer = static_cast<const unsigned char *>(memory);
			unsigned char * dest_buffer = static_cast<unsigned char *>(_buffer);

			for( size_t j = 0; j != m_dataInfo.height; ++j )
			{
				stdex::memorycopy( dest_buffer, source_buffer, m_dataInfo.width * m_dataInfo.channels );

				source_buffer += m_dataInfo.width * m_dataInfo.channels;
				dest_buffer += m_options.pitch;
			}	
		}

		return uncompress_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
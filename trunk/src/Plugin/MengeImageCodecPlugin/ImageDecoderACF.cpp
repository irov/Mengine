#	include "ImageDecoderACF.h"
#   include "ImageVerifyACF.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Core/CacheMemoryBuffer.h"
#	include "Core/Stream.h"

#   include "Config/Blobject.h"

#	include "stdex/memorycopy.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderACF::ImageDecoderACF()
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
			->getArchivator( STRINGIZE_STRING_LOCAL(m_serviceProvider, "lz4") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderACF::_prepareData()
	{
		if( Helper::loadStreamMagicHeader( m_serviceProvider, m_stream, GET_MAGIC_NUMBER(MAGIC_ACF), GET_MAGIC_VERSION(MAGIC_ACF) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::_prepareData invalid load magic header"
				);

			return false;
		}

		uint32_t width;
        m_stream->read( &width, sizeof(width) );

		uint32_t height;
		m_stream->read( &height, sizeof(height) );

		uint32_t mipmaps;
		m_stream->read( &mipmaps, sizeof(mipmaps) );

		m_dataInfo.width = width;
		m_dataInfo.height = height;
        m_dataInfo.channels = 1;
		m_dataInfo.format = PF_A8;
		m_dataInfo.mipmaps = mipmaps;
      
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderACF::decode( void * _buffer, size_t _bufferSize )
	{
		(void)_bufferSize;

		size_t dataSize;
		if( Helper::loadStreamArchiveBufferSize( m_serviceProvider, m_stream, dataSize ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::decode invalid load data size"
				);

			return 0;
		}

		if( dataSize > _bufferSize )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::decode overrlow data size %d need %d"
				, dataSize
				, _bufferSize
				);

			return 0;
		}
		
		if( m_options.pitch * m_dataInfo.height == dataSize )
		{		
			if( Helper::loadStreamArchiveInplace( m_serviceProvider, m_stream, m_archivator, _buffer, _bufferSize ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::decode invalid load"
					);

				return 0;
			}
		}
		else
		{
			CacheMemoryBuffer buffer(m_serviceProvider, dataSize, "ImageDecoderACF::decode");
			void * memory = buffer.getMemory();

			if( Helper::loadStreamArchiveInplace( m_serviceProvider, m_stream, m_archivator, memory, dataSize ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::decode invalid load"
					);

				return 0;
			}

			const unsigned char * source_buffer = static_cast<const unsigned char *>(memory);
			unsigned char * dest_buffer = static_cast<unsigned char *>(_buffer);

			for( uint32_t j = 0; j != m_dataInfo.height; ++j )
			{
				stdex::memorycopy( dest_buffer, source_buffer, m_dataInfo.width * m_dataInfo.channels );

				source_buffer += m_dataInfo.width * m_dataInfo.channels;
				dest_buffer += m_options.pitch;
			}	
		}

		return _bufferSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
#	include "ImageDecoderACF.h"
#   include "ImageVerifyACF.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Config/Blobject.h"

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
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->decompressStream( m_archivator, m_stream, m_compress_size, _buffer, m_uncompress_size, uncompress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderACF::decode uncompress failed"
                );

            return 0;
        }		

		return uncompress_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
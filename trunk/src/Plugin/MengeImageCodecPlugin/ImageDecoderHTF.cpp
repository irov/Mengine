#	include "ImageDecoderHTF.h"
#   include "ImageVerifyHTF.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Config/Blobject.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static PixelFormat s_convertFormat( uint32_t _format )
    {
        switch(_format)
        {
        case 1:
            return PF_DXT1;
        case 2:
            return PF_ETC1;
		case 3:
			return PF_PVRTC4_RGB;
        }

        return PF_UNKNOWN;
    };
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderHTF::ImageDecoderHTF()
		: m_uncompress_size(0)
		, m_compress_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderHTF::~ImageDecoderHTF()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderHTF::_initialize()
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
	bool ImageDecoderHTF::_prepareData()
	{
        uint32_t magic;
        m_stream->read( &magic, sizeof(magic) );

        if( magic != GET_MAGIC_NUMBER(MAGIC_HTF) )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderHTF::initialize invalid htf file magic number" 
                );

            return false;
        }

		uint32_t version;
		m_stream->read( &version, sizeof(version) );

		if( version != GET_MAGIC_VERSION(MAGIC_HTF) )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderHTF::initialize invalid htf file magic version" 
				);

			return false;
		}

		uint32_t width;
        m_stream->read( &width, sizeof(width) );

		uint32_t height;
		m_stream->read( &height, sizeof(height) );

		uint32_t format;
		m_stream->read( &format, sizeof(format) );

		m_stream->read( &m_uncompress_size, sizeof(m_uncompress_size) );
		m_stream->read( &m_compress_size, sizeof(m_compress_size) );

		m_dataInfo.width = width;
		m_dataInfo.height = height;
        m_dataInfo.channels = 3;
		
        m_dataInfo.format = s_convertFormat( format );

		m_dataInfo.size = Helper::getTextureMemorySize( m_dataInfo.width, m_dataInfo.height, m_dataInfo.depth, m_dataInfo.format );
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderHTF::decode( void * _buffer, size_t _bufferSize )
	{
        if( _bufferSize != m_uncompress_size )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderHTF::decode uncompress failed %d != %d"
				, _bufferSize
				, m_uncompress_size
				);

			return 0;
		}

        size_t uncompress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->decompressStream( m_archivator, m_stream, m_compress_size, _buffer, m_uncompress_size, uncompress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderHTF::decode uncompress failed"
                );

            return 0;
        }		

		return uncompress_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
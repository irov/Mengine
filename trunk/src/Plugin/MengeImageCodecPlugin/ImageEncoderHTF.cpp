#	include "ImageEncoderHTF.h"
#   include "ImageVerifyHTF.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Config/Blobject.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static uint32_t s_convertFormat( PixelFormat _format )
	{
		switch( _format )
		{
		case PF_DXT1:
			return 1;
		case PF_ETC1:
			return 2;
		case PF_PVRTC4_RGB:
			return 3;
		}

		return 0;
	};
	//////////////////////////////////////////////////////////////////////////
	ImageEncoderHTF::ImageEncoderHTF()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageEncoderHTF::~ImageEncoderHTF()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageEncoderHTF::_initialize()
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
	size_t ImageEncoderHTF::encode( const void * _buffer, const CodecDataInfo* _bufferDataInfo )
	{
		const ImageCodecDataInfo * dataInfo = static_cast<const ImageCodecDataInfo *>( _bufferDataInfo );
				
		magic_number_type magic_number = GET_MAGIC_NUMBER(MAGIC_HTF);
        m_stream->write( &magic_number, sizeof(magic_number) );
			
		magic_version_type magic_version = GET_MAGIC_VERSION(MAGIC_HTF);
        m_stream->write( &magic_version, sizeof(magic_version) );

		uint32_t minsize;

		switch( dataInfo->format )
		{
		case PF_DXT1:
		case PF_ETC1:
			{
				minsize = 4;
			}break;
		case PF_PVRTC4_RGB:
			{
				minsize = 2;
			}break;
		default:
			{
				LOGGER_ERROR(m_serviceProvider)("ImageEncoderHTF::encode invalid format %d"
					, dataInfo->format
					);

				return 0;
			}break;
		}

		uint32_t width = dataInfo->width > minsize ? dataInfo->width : minsize;
		uint32_t height = dataInfo->height > minsize ? dataInfo->height : minsize;

        m_stream->write( &width, sizeof(width) );		
        m_stream->write( &height, sizeof(height) );

		uint32_t format = s_convertFormat( dataInfo->format );
        m_stream->write( &format, sizeof(format) );
		                
		size_t uncompressSize = dataInfo->size;
		MemoryInputPtr compress_memory = ARCHIVE_SERVICE(m_serviceProvider)
            ->compressBuffer( m_archivator, _buffer, uncompressSize );

		if( compress_memory == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageEncoderHTF::encode invalid compress"
                );

            return 0;
        }

		uint32_t compressSize;
		const void * compressBuffer = compress_memory->getMemory( compressSize );
		
		m_stream->write( &uncompressSize, sizeof(uncompressSize) );
        m_stream->write( &compressSize, sizeof(compressSize) );

        m_stream->write( compressBuffer, compressSize );

		return compressSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

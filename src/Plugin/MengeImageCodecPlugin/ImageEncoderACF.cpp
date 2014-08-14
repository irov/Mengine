#	include "ImageEncoderACF.h"
#   include "ImageVerifyACF.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Config/Blobject.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ImageEncoderACF::ImageEncoderACF()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageEncoderACF::~ImageEncoderACF()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageEncoderACF::_initialize()
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
	size_t ImageEncoderACF::encode( const void * _buffer, const CodecDataInfo* _bufferDataInfo )
	{
		const ImageCodecDataInfo * dataInfo = static_cast<const ImageCodecDataInfo *>(_bufferDataInfo);
				
		magic_number_type magic_number = GET_MAGIC_NUMBER(MAGIC_ACF);
        m_stream->write( &magic_number, sizeof(magic_number) );

		magic_version_type magic_version = GET_MAGIC_VERSION(MAGIC_ACF);
        m_stream->write( &magic_version, sizeof(magic_version) );

        m_stream->write( &dataInfo->width, sizeof(dataInfo->width) );
        m_stream->write( &dataInfo->height, sizeof(dataInfo->height) );

		size_t uncompressSize = dataInfo->size;
		MemoryInputPtr compress_memory = ARCHIVE_SERVICE(m_serviceProvider)
            ->compressBuffer( m_archivator, _buffer, uncompressSize );

		if( compress_memory == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("PickEncoderHIT::encode invalid compress"
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

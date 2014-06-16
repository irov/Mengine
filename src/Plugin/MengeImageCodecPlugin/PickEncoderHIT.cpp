#	include "PickEncoderHIT.h"
#   include "PickVerifyHIT.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Config/Blobject.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PickEncoderHIT::PickEncoderHIT()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PickEncoderHIT::~PickEncoderHIT()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PickEncoderHIT::_initialize()
	{
		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "zip") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t PickEncoderHIT::encode( const void * _buffer, const CodecDataInfo* _bufferDataInfo )
	{
		const PickCodecDataInfo * dataInfo = static_cast<const PickCodecDataInfo*>( _bufferDataInfo );

		
		magic_number_type magic_number = GET_MAGIC_NUMBER(MAGIC_HIT);
        m_stream->write( &magic_number, sizeof(magic_number) );
			
		magic_version_type magic_version = GET_MAGIC_VERSION(MAGIC_HIT);
        m_stream->write( &magic_version, sizeof(magic_version) );

        m_stream->write( &dataInfo->width, sizeof(dataInfo->width) );
        m_stream->write( &dataInfo->height, sizeof(dataInfo->height) );
        m_stream->write( &dataInfo->mipmaplevel, sizeof(dataInfo->mipmaplevel) );
		m_stream->write( &dataInfo->mipmapsize, sizeof(dataInfo->mipmapsize) );
                
		MemoryInputPtr compress_memory = ARCHIVE_SERVICE(m_serviceProvider)
            ->compress( m_archivator, _buffer, dataInfo->mipmapsize );

		if( compress_memory == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("PickEncoderHIT::encode invalid compress"
                );

            return 0;
        }

		uint32_t compressSize;
		const void * compressBuffer = compress_memory->getMemory( compressSize );
		
        m_stream->write( &compressSize, sizeof(compressSize) );
        m_stream->write( compressBuffer, compressSize );

		return compressSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

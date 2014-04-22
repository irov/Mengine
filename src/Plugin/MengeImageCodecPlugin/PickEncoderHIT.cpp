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
	size_t PickEncoderHIT::encode( const void * _buffer, const CodecDataInfo* _bufferDataInfo )
	{
		const PickCodecDataInfo * dataInfo = static_cast<const PickCodecDataInfo*>( _bufferDataInfo );

        m_stream->write( &hit_magic, sizeof(hit_magic) );
        m_stream->write( &hit_version, sizeof(hit_version) );

        m_stream->write( &dataInfo->width, sizeof(dataInfo->width) );
        m_stream->write( &dataInfo->height, sizeof(dataInfo->height) );
        m_stream->write( &dataInfo->mipmaplevel, sizeof(dataInfo->mipmaplevel) );
		m_stream->write( &dataInfo->mipmapsize, sizeof(dataInfo->mipmapsize) );
                
		MemoryInputPtr compress_memory = ARCHIVE_SERVICE(m_serviceProvider)
            ->compress( Helper::stringizeString(m_serviceProvider, "zip"), _buffer, dataInfo->mipmapsize );

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
	bool PickEncoderHIT::_initialize()
	{
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

#	include "PickEncoderHIT.h"
#   include "PickVerifyHIT.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ArchiveInterface.h"

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
	unsigned int PickEncoderHIT::encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo )
	{
		const PickCodecDataInfo* dataInfo = static_cast<const PickCodecDataInfo*>( _bufferDataInfo );

        m_stream->write( &hit_magic, sizeof(hit_magic) );
        m_stream->write( &hit_version, sizeof(hit_version) );
        m_stream->write( &dataInfo->width, sizeof(dataInfo->width) );
        m_stream->write( &dataInfo->height, sizeof(dataInfo->height) );
        m_stream->write( &dataInfo->mipmaplevel, sizeof(dataInfo->mipmaplevel) );
        m_stream->write( &dataInfo->mipmapsize, sizeof(dataInfo->mipmapsize) );

        size_t compressSize2 = ARCHIVE_SERVICE(m_serviceProvider)
            ->compressBound( dataInfo->mipmapsize );

        TBlobject::value_type * compressBuffer = new TBlobject::value_type[compressSize2];

        size_t compressSize;
        if( ARCHIVE_SERVICE(m_serviceProvider)->compress( compressBuffer, compressSize2, compressSize, _buffer, dataInfo->mipmapsize ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("PickEncoderHIT::encode invalid compress"
                );

            return 0;
        }

        m_stream->write( &compressSize, sizeof(compressSize) );
        m_stream->write( compressBuffer, compressSize );

        delete [] compressBuffer;

		return compressSize;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PickEncoderHIT::_initialize()
	{
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

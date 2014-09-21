#	include "PickEncoderHIT.h"
#   include "PickVerifyHIT.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/Stream.h"

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
		if( Helper::writeStreamMagicHeader( m_serviceProvider, m_stream, GET_MAGIC_NUMBER(MAGIC_HIT), GET_MAGIC_VERSION(MAGIC_HIT) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("PickEncoderHIT::encode invalid write magic header"
				);

			return 0;
		}

		const PickCodecDataInfo * dataInfo = static_cast<const PickCodecDataInfo*>( _bufferDataInfo );

        m_stream->write( &dataInfo->width, sizeof(dataInfo->width) );
        m_stream->write( &dataInfo->height, sizeof(dataInfo->height) );
        m_stream->write( &dataInfo->mipmaplevel, sizeof(dataInfo->mipmaplevel) );
		m_stream->write( &dataInfo->mipmapsize, sizeof(dataInfo->mipmapsize) );

		const void * write_buffer = _buffer;
		size_t write_size = (size_t)dataInfo->mipmapsize;
              
		if( Helper::writeStreamArchiveBuffer( m_serviceProvider, m_stream, m_archivator, false, write_buffer, write_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("PickEncoderHIT::encode invalid write magic header"
				);

			return 0;
		}

		return write_size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

#	include "ImageEncoderACF.h"
#   include "ImageVerifyACF.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/Stream.h"

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
		if( Helper::writeStreamMagicHeader( m_serviceProvider, m_stream, GET_MAGIC_NUMBER(MAGIC_ACF), GET_MAGIC_VERSION(MAGIC_ACF) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageEncoderACF::encode invalid write magic header"
				);

			return 0;
		}

		const ImageCodecDataInfo * dataInfo = static_cast<const ImageCodecDataInfo *>(_bufferDataInfo);
				
        m_stream->write( &dataInfo->width, sizeof(dataInfo->width) );
        m_stream->write( &dataInfo->height, sizeof(dataInfo->height) );

		if( Helper::writeStreamArchiveBuffer( m_serviceProvider, m_stream, m_archivator, false, _buffer, dataInfo->size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageEncoderACF::encode invalid write buffer"
				);

			return 0;
		}

		return dataInfo->size;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

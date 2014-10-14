#	include "ImageEncoderHTF.h"
#   include "ImageVerifyHTF.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/Stream.h"

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
	size_t ImageEncoderHTF::encode( const void * _buffer, const CodecDataInfo * _dataInfo )
	{
		if( Helper::writeStreamMagicHeader( m_serviceProvider, m_stream, GET_MAGIC_NUMBER(MAGIC_HTF), GET_MAGIC_VERSION(MAGIC_HTF) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageEncoderHTF::encode invalid write magic header"
				);

			return 0;
		}

		const ImageCodecDataInfo * dataInfo = static_cast<const ImageCodecDataInfo *>(_dataInfo);

		uint32_t width = dataInfo->width;
		uint32_t height = dataInfo->height;

		width = width > 4 ? width : 4;
		height = height > 4 ? height : 4;

        m_stream->write( &width, sizeof(width) );
        m_stream->write( &height, sizeof(height) );

		uint32_t format = s_convertFormat( dataInfo->format );
        m_stream->write( &format, sizeof(format) );

		uint32_t mipmaps = dataInfo->mipmaps;
		m_stream->write( &mipmaps, sizeof(mipmaps) );

		const unsigned char * mipmap_buffer = reinterpret_cast<const unsigned char *>(_buffer);

		for( uint32_t i = 0; i != mipmaps; ++i )
		{             
			size_t mipmap_size = dataInfo->getMipMapSize( i );

			if( Helper::writeStreamArchiveBuffer( m_serviceProvider, m_stream, m_archivator, false, mipmap_buffer, mipmap_size ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ImageEncoderHTF::encode invalid write buffer"
					);

				return 0;
			}

			mipmap_buffer += mipmap_size;
		}

		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

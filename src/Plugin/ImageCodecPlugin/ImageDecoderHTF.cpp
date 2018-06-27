#include "ImageDecoderHTF.h"
#include "ImageVerifyHTF.h"

#include "Interface/StringizeInterface.h"

#include "Core/Stream.h"

#include "Logger/Logger.h"

namespace Mengine
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
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderHTF::~ImageDecoderHTF()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderHTF::_initialize()
	{		
        const ArchivatorInterfacePtr & archivator = ARCHIVE_SERVICE()
			->getArchivator( STRINGIZE_STRING_LOCAL("lz4") );

		if( archivator == nullptr )
		{
			return false;
		}

        m_archivator = archivator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ImageDecoderHTF::_prepareData()
	{
		if( Helper::loadStreamMagicHeader( m_stream, GET_MAGIC_NUMBER(MAGIC_HTF), GET_MAGIC_VERSION(MAGIC_HTF) ) == false )
		{
			LOGGER_ERROR("ImageDecoderHTF::_prepareData invalid load magic header"
				);

			return false;
		}

		uint32_t width;
        m_stream->read( &width, sizeof(width) );

		uint32_t height;
		m_stream->read( &height, sizeof(height) );

		uint32_t format;
		m_stream->read( &format, sizeof(format) );

		uint32_t mipmaps;
		m_stream->read( &mipmaps, sizeof(mipmaps) );
				
		m_dataInfo.width = width;
		m_dataInfo.height = height;
        m_dataInfo.channels = 3;
		
        m_dataInfo.format = s_convertFormat( format );
		m_dataInfo.mipmaps = mipmaps;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderHTF::_decode( void * _buffer, size_t _bufferSize )
	{
		if( Helper::loadStreamArchiveInplace( m_stream, m_archivator, _buffer, _bufferSize, __FILE__, __LINE__ ) == false )
		{
			LOGGER_ERROR("ImageDecoderHTF::decode invalid load"
				);

			return 0;
		}
		
		return _bufferSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	

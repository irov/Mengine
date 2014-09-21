#	include "ImageDecoderHTF.h"
#   include "ImageVerifyHTF.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Core/Stream.h"

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
		if( Helper::loadStreamMagicHeader( m_serviceProvider, m_stream, GET_MAGIC_NUMBER(MAGIC_HTF), GET_MAGIC_VERSION(MAGIC_HTF) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageDecoderHTF::_prepareData invalid load magic header"
				);

			return false;
		}

		uint32_t width;
        m_stream->read( &width, sizeof(width) );

		uint32_t height;
		m_stream->read( &height, sizeof(height) );

		uint32_t format;
		m_stream->read( &format, sizeof(format) );

		m_dataInfo.width = width;
		m_dataInfo.height = height;
        m_dataInfo.channels = 3;
		
        m_dataInfo.format = s_convertFormat( format );

		m_dataInfo.size = Helper::getTextureMemorySize( m_dataInfo.width, m_dataInfo.height, m_dataInfo.channels, m_dataInfo.depth, m_dataInfo.format );
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ImageDecoderHTF::decode( void * _buffer, size_t _bufferSize )
	{
		if( Helper::loadStreamArchiveInplace( m_serviceProvider, m_stream, m_archivator, _buffer, _bufferSize ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageDecoderHTF::decode invalid load"
                );

            return 0;
        }		

		return _bufferSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
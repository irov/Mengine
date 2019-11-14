#include "ImageDecoderETC1.h"

#include "Kernel/Logger.h"

#include <string.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static uint16_t readBEUint16( const uint8_t * pIn )
    {
        return (pIn[0] << 8) | pIn[1];
    }
    //////////////////////////////////////////////////////////////////////////
    static const uint16_t ETC1_PKM_FORMAT_OFFSET = 6;
    static const uint16_t ETC1_PKM_ENCODED_WIDTH_OFFSET = 8;
    static const uint16_t ETC1_PKM_ENCODED_HEIGHT_OFFSET = 10;
    static const uint16_t ETC1_PKM_WIDTH_OFFSET = 12;
    static const uint16_t ETC1_PKM_HEIGHT_OFFSET = 14;
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderETC1::ImageDecoderETC1()
        : m_needSeek( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderETC1::~ImageDecoderETC1()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderETC1::_decode( void * _buffer, size_t _bufferSize )
    {
        if( m_needSeek == true )
        {
            m_stream->seek( sizeof( ETC1Header ) );
        }

        m_stream->read( _buffer, _bufferSize );
        m_needSeek = true;
        return _bufferSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderETC1::_prepareData()
    {
        if( m_needSeek )
        {
            m_stream->seek( 0 );
        }

        m_stream->read( &m_header, sizeof( ETC1Header ) );
        m_needSeek = false;

        if( strncmp( m_header.tag, "PKM 10", 6 ) != 0 )
        {
            LOGGER_ERROR( "Bad or not ETC1 file" );

            return false;
        }

        uint8_t * buffer = reinterpret_cast<uint8_t *>(&m_header);
        m_header.format = readBEUint16( buffer + ETC1_PKM_FORMAT_OFFSET );
        m_header.texHeight = readBEUint16( buffer + ETC1_PKM_ENCODED_HEIGHT_OFFSET );
        m_header.texWidth = readBEUint16( buffer + ETC1_PKM_ENCODED_WIDTH_OFFSET );
        m_header.origHeight = readBEUint16( buffer + ETC1_PKM_HEIGHT_OFFSET );
        m_header.origWidth = readBEUint16( buffer + ETC1_PKM_WIDTH_OFFSET );

        bool isValid = m_header.texWidth >= m_header.origWidth
            && m_header.texWidth - m_header.origWidth < 4
            && m_header.texHeight >= m_header.origHeight
            && m_header.texHeight - m_header.origHeight < 4;

        m_dataInfo.channels = 3;
        m_dataInfo.depth = 1;
        //m_dataInfo.format = PF_ETC1;
        m_dataInfo.width = m_header.texWidth;
        m_dataInfo.height = m_header.texHeight;

        m_dataInfo.mipmaps = 1 + m_header.format;

        return isValid;
    }
}
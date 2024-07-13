#include "ImageDecoderETC1.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        static uint16_t readBEUint16( const uint8_t * pIn )
        {
            //cppcheck-suppress objectIndex
            return (pIn[0] << 8) | pIn[1];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint16_t ETC1_PKM_FORMAT_OFFSET = 6;
    static constexpr uint16_t ETC1_PKM_ENCODED_WIDTH_OFFSET = 8;
    static constexpr uint16_t ETC1_PKM_ENCODED_HEIGHT_OFFSET = 10;
    static constexpr uint16_t ETC1_PKM_WIDTH_OFFSET = 12;
    static constexpr uint16_t ETC1_PKM_HEIGHT_OFFSET = 14;
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderETC1::ImageDecoderETC1()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderETC1::~ImageDecoderETC1()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderETC1::_decode( const DecoderData * _data )
    {
        void * buffer = _data->buffer;
        size_t size = _data->size;

        const InputStreamInterfacePtr & stream = this->getStream();

        size_t byte = stream->read( buffer, size );

        return byte;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderETC1::_prepareData()
    {
        const InputStreamInterfacePtr & stream = this->getStream();

        stream->read( &m_header, sizeof( ETC1Header ) );

        MENGINE_ASSERTION_FATAL( MENGINE_STRNCMP( m_header.tag, "PKM 10", 6 ) == 0, "Bad or not ETC1 file" );
        MENGINE_ASSERTION_FATAL( m_header.texWidth >= m_header.origWidth, "Bad ETC1 file" );
        MENGINE_ASSERTION_FATAL( m_header.texWidth - m_header.origWidth < 4, "Bad ETC1 file" );
        MENGINE_ASSERTION_FATAL( m_header.texHeight >= m_header.origHeight, "Bad ETC1 file" );
        MENGINE_ASSERTION_FATAL( m_header.texHeight - m_header.origHeight < 4, "Bad ETC1 file" );

        const uint8_t * buffer = reinterpret_cast<const uint8_t *>(&m_header);

        m_header.format = Detail::readBEUint16( buffer + ETC1_PKM_FORMAT_OFFSET );
        m_header.texHeight = Detail::readBEUint16( buffer + ETC1_PKM_ENCODED_HEIGHT_OFFSET );
        m_header.texWidth = Detail::readBEUint16( buffer + ETC1_PKM_ENCODED_WIDTH_OFFSET );
        m_header.origHeight = Detail::readBEUint16( buffer + ETC1_PKM_HEIGHT_OFFSET );
        m_header.origWidth = Detail::readBEUint16( buffer + ETC1_PKM_WIDTH_OFFSET );

        m_dataInfo.mipmaps = 1 + m_header.format;
        m_dataInfo.width = m_header.texWidth;
        m_dataInfo.height = m_header.texHeight;
        m_dataInfo.format = PF_R8G8B8;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
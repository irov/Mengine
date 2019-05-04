#include "ImageEncoderACF.h"
#include "ImageVerifyACF.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Stream.h"

#include "Kernel/Logger.h"

namespace Mengine
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
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageEncoderACF::encode( const void * _buffer, size_t _size, const CodecDataInfo* _bufferDataInfo )
    {
        MENGINE_UNUSED( _size );

        if( Helper::writeStreamMagicHeader( m_stream, GET_MAGIC_NUMBER( MAGIC_ACF ), GET_MAGIC_VERSION( MAGIC_ACF ) ) == false )
        {
            LOGGER_ERROR( "ImageEncoderACF::encode invalid write magic header"
            );

            return 0;
        }

        const ImageCodecDataInfo * dataInfo = static_cast<const ImageCodecDataInfo *>(_bufferDataInfo);

        uint32_t width = dataInfo->width;
        m_stream->write( &width, sizeof( width ) );

        uint32_t height = dataInfo->height;
        m_stream->write( &height, sizeof( height ) );

        uint32_t mipmaps = dataInfo->mipmaps;
        m_stream->write( &mipmaps, sizeof( mipmaps ) );

        const uint8_t * mipmap_buffer = reinterpret_cast<const uint8_t *>(_buffer);

        for( uint32_t i = 0; i != mipmaps; ++i )
        {
            uint32_t mipmap_size = dataInfo->getMipMapSize( i );

            if( Helper::writeStreamArchiveBuffer( m_stream, m_archivator, false, mipmap_buffer, mipmap_size, EAC_BEST ) == false )
            {
                LOGGER_ERROR( "ImageEncoderACF::encode invalid write buffer"
                );

                return 0;
            }

            mipmap_buffer += mipmap_size;
        }

        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
}

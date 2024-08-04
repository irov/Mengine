#include "ImageEncoderHTF.h"
#include "ImageVerifyHTF.h"

#include "Kernel/StreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ImageCodecHelper.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static uint32_t s_convertFormat( EPixelFormat _format )
    {
        switch( _format )
        {
        case PF_DXT1:
            return 1;
        case PF_ETC1:
            return 2;
        case PF_PVRTC4_RGB:
            return 3;
        default:
            {
            }break;
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
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageEncoderHTF::_finalize()
    {
        m_archivator = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageEncoderHTF::encode( const EncoderData * _encoderData, const CodecDataInfo * _dataInfo )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _encoderData );
        MENGINE_ASSERTION_TYPE( _encoderData, const ImageEncoderData * );

        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const ImageCodecDataInfo * );

        if( Helper::writeStreamMagicHeader( m_stream, GET_MAGIC_NUMBER( MAGIC_HTF ), GET_MAGIC_VERSION( MAGIC_HTF ) ) == false )
        {
            LOGGER_ERROR( "invalid write magic header" );

            return 0;
        }

        const ImageCodecDataInfo * dataInfo = static_cast<const ImageCodecDataInfo *>(_dataInfo);

        uint32_t width = dataInfo->width;
        uint32_t height = dataInfo->height;

        width = width > 4 ? width : 4;
        height = height > 4 ? height : 4;

        m_stream->write( &width, sizeof( width ) );
        m_stream->write( &height, sizeof( height ) );

        uint32_t format = s_convertFormat( dataInfo->format );
        m_stream->write( &format, sizeof( format ) );

        uint32_t mipmaps = dataInfo->mipmaps;
        m_stream->write( &mipmaps, sizeof( mipmaps ) );

        const uint8_t * mipmap_buffer = reinterpret_cast<const uint8_t *>(_encoderData->buffer);

        for( uint32_t i = 0; i != mipmaps; ++i )
        {
            size_t mipmap_size = Helper::getImageCodecDataMipMapSize( dataInfo, i );

            if( Helper::writeStreamArchiveBuffer( m_stream, m_archivator, false, mipmap_buffer, mipmap_size, EAC_BEST ) == false )
            {
                LOGGER_ERROR( "invalid write buffer" );

                return 0;
            }

            mipmap_buffer += mipmap_size;
        }

        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
}

#include "ImageEncoderACF.h"
#include "ImageVerifyACF.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Stream.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ImageCodecHelper.h"

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

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageEncoderACF::encode( const EncoderData * _encoderData, const CodecDataInfo * _dataInfo )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const ImageCodecDataInfo * );

        const ImageCodecDataInfo * dataInfo = static_cast<const ImageCodecDataInfo *>(_dataInfo);

        if( Helper::writeStreamMagicHeader( m_stream, GET_MAGIC_NUMBER( MAGIC_ACF ), GET_MAGIC_VERSION( MAGIC_ACF ) ) == false )
        {
            LOGGER_ERROR( "invalid write magic header" );

            return 0;
        }        

        uint32_t width = dataInfo->width;
        m_stream->write( &width, sizeof( width ) );

        uint32_t height = dataInfo->height;
        m_stream->write( &height, sizeof( height ) );

        uint32_t mipmaps = dataInfo->mipmaps;
        m_stream->write( &mipmaps, sizeof( mipmaps ) );

        const uint8_t * mipmap_buffer = reinterpret_cast<const uint8_t *>(_encoderData->buffer);

        for( uint32_t i = 0; i != mipmaps; ++i )
        {
            uint32_t mipmap_size = Helper::getImageCodecDataMipMapSize( dataInfo, i );

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

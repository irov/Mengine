#include "ImageDecoderACF.h"
#include "ImageVerifyACF.h"

#include "Kernel/StreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/MemoryCopy.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderACF::ImageDecoderACF()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderACF::~ImageDecoderACF()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderACF::_initialize()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderACF::_prepareData()
    {
        const InputStreamInterfacePtr & stream = this->getStream();

        if( Helper::readStreamMagicHeader( stream, GET_MAGIC_NUMBER( MAGIC_ACF ), GET_MAGIC_VERSION( MAGIC_ACF ) ) == false )
        {
            LOGGER_ERROR( "invalid load magic header" );

            return false;
        }

        uint32_t width;
        stream->read( &width, sizeof( width ) );

        uint32_t height;
        stream->read( &height, sizeof( height ) );

        uint32_t mipmaps;
        stream->read( &mipmaps, sizeof( mipmaps ) );

        m_dataInfo.width = width;
        m_dataInfo.height = height;
        m_dataInfo.format = PF_L8;
        m_dataInfo.mipmaps = mipmaps;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderACF::_decode( const DecoderData * _decoderData )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _decoderData );
        MENGINE_ASSERTION_TYPE( _decoderData, const ImageDecoderData * );

        const ImageDecoderData * decoderData = static_cast<const ImageDecoderData *>(_decoderData);

        const InputStreamInterfacePtr & stream = this->getStream();

        void * dataBuffer = decoderData->buffer;
        size_t dataSize = decoderData->size;
        size_t dataPitch = decoderData->pitch;

        size_t streamSize;
        if( Helper::readStreamArchiveBufferSize( stream, &streamSize ) == false )
        {
            LOGGER_ERROR( "invalid load data size" );

            return 0;
        }

        if( streamSize > dataSize )
        {
            LOGGER_ERROR( "overrlow data size %zu need %zu"
                , streamSize
                , dataSize
            );

            return 0;
        }

        if( dataPitch * m_dataInfo.height == streamSize )
        {
            if( Helper::readStreamArchiveInplace( stream, m_archivator, dataBuffer, dataSize, nullptr, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load" );

                return 0;
            }
        }
        else
        {
            MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( streamSize, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer );

            void * memory = buffer->getBuffer();

            if( Helper::readStreamArchiveInplace( stream, m_archivator, memory, streamSize, nullptr, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load" );

                return 0;
            }

            const uint8_t * source_buffer = static_cast<const uint8_t *>(memory);
            uint8_t * dest_buffer = static_cast<uint8_t *>(dataBuffer);

            uint32_t channels = Helper::getPixelFormatChannels( m_dataInfo.format );
            uint32_t linesize = m_dataInfo.width * channels;

            for( uint32_t j = 0; j != m_dataInfo.height; ++j )
            {
                Helper::memoryCopy( dest_buffer, 0, source_buffer, 0, linesize );

                source_buffer += linesize;
                dest_buffer += dataPitch;
            }
        }

        return dataSize;
    }
    //////////////////////////////////////////////////////////////////////////
}

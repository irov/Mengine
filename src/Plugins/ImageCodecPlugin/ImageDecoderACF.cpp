#include "ImageDecoderACF.h"
#include "ImageVerifyACF.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Stream.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "stdex/memorycopy.h"

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
        if( Helper::loadStreamMagicHeader( m_stream, GET_MAGIC_NUMBER( MAGIC_ACF ), GET_MAGIC_VERSION( MAGIC_ACF ) ) == false )
        {
            LOGGER_ERROR( "invalid load magic header" );

            return false;
        }

        uint32_t width;
        m_stream->read( &width, sizeof( width ) );

        uint32_t height;
        m_stream->read( &height, sizeof( height ) );

        uint32_t mipmaps;
        m_stream->read( &mipmaps, sizeof( mipmaps ) );

        m_dataInfo.width = width;
        m_dataInfo.height = height;
        m_dataInfo.channels = 1;
        m_dataInfo.format = PF_A8;
        m_dataInfo.mipmaps = mipmaps;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderACF::_decode( void * const _buffer, size_t _bufferSize )
    {
        size_t dataSize;
        if( Helper::loadStreamArchiveBufferSize( m_stream, &dataSize ) == false )
        {
            LOGGER_ERROR( "invalid load data size" );

            return 0;
        }

        if( dataSize > _bufferSize )
        {
            LOGGER_ERROR( "overrlow data size %zu need %zu"
                , dataSize
                , _bufferSize
            );

            return 0;
        }

        if( m_options.pitch * m_dataInfo.height == dataSize )
        {
            if( Helper::loadStreamArchiveInplace( m_stream, m_archivator, _buffer, _bufferSize, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load" );

                return 0;
            }
        }
        else
        {
            MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( dataSize, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer );

            void * memory = buffer->getBuffer();

            if( Helper::loadStreamArchiveInplace( m_stream, m_archivator, memory, dataSize, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load" );

                return 0;
            }

            const uint8_t * source_buffer = static_cast<const uint8_t *>(memory);
            uint8_t * dest_buffer = static_cast<uint8_t *>(_buffer);

            for( uint32_t j = 0; j != m_dataInfo.height; ++j )
            {
                stdex::memorycopy( dest_buffer, 0, source_buffer, m_dataInfo.width * m_dataInfo.channels );

                source_buffer += m_dataInfo.width * m_dataInfo.channels;
                dest_buffer += m_options.pitch;
            }
        }

        return _bufferSize;
    }
    //////////////////////////////////////////////////////////////////////////
}

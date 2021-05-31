#include "ImageDecoderArchive.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/PixelFormatHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderArchive::ImageDecoderArchive()
        : m_uncompressSize( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderArchive::~ImageDecoderArchive()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoderArchive::setArchivator( const ArchivatorInterfacePtr & _archivator )
    {
        m_archivator = _archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    const ArchivatorInterfacePtr & ImageDecoderArchive::getArchivator() const
    {
        return m_archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoderArchive::setUncompressSize( size_t _size )
    {
        m_uncompressSize = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderArchive::getUncompressSize() const
    {
        return m_uncompressSize;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderArchive::_decode( const DecoderData * _data )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _data );
        MENGINE_ASSERTION_TYPE( _data, const ImageDecoderData * );

        const ImageDecoderData * imageData = static_cast<const ImageDecoderData *>(_data);

        void * buffer = imageData->buffer;
        size_t size = imageData->size;
        size_t pitch = imageData->pitch;

        MENGINE_ASSERTION_FATAL( size >= pitch * m_dataInfo.height, "invalid bufferSize %zu != (%zu * %u)"
            , size
            , pitch
            , m_dataInfo.height
        );

        m_stream->seek( 0 );

        size_t read_byte = 0;

        uint32_t dataChannels = Helper::getPixelFormatChannels( m_dataInfo.format );
        uint32_t optionChannels = Helper::getPixelFormatChannels( imageData->format );

        if( dataChannels == 4 && optionChannels == 4 )
        {
            read_byte = this->decodeData_( buffer, size, pitch );
        }
        else if( dataChannels == 3 && optionChannels == 4 )
        {
            read_byte = this->decodeData_( buffer, size, pitch );

            this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, buffer, pitch );
        }
        else
        {
            LOGGER_ERROR( "invalid channels data %u options %u"
                , dataChannels
                , optionChannels
            );

            return 0;
        }

        return read_byte;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderArchive::decodeData_( void * const _buffer, size_t _bufferSize, size_t _pitch ) const
    {
        MENGINE_UNUSED( _bufferSize );

        size_t decodyByte = 0;

        size_t stream_size;
        void * stream_memory;
        if( m_stream->memory( &stream_memory, &stream_size ) == false )
        {
            MemoryInterfacePtr buffer = Helper::createMemoryCacheStream( m_stream, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer );

            const void * cache_buffer = buffer->getBuffer();
            size_t cache_size = buffer->getSize();

            decodyByte = this->decompressData_( cache_buffer, cache_size, _buffer, m_uncompressSize, _pitch );
        }
        else
        {
            decodyByte = this->decompressData_( stream_memory, stream_size, _buffer, m_uncompressSize, _pitch );
        }

        return decodyByte;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderArchive::decompressData_( const void * _source, size_t _sourceSize, void * const _dest, size_t _capacityDest, size_t _pitch ) const
    {
        size_t read_byte = 0;

        uint32_t channels = Helper::getPixelFormatChannels( m_dataInfo.format );

        if( _pitch == m_dataInfo.width * channels )
        {
            size_t decompressSize;
            if( m_archivator->decompress( _dest, _capacityDest, _source, _sourceSize, &decompressSize ) == false )
            {
                return 0;
            }

            read_byte = decompressSize;
        }
        else
        {
            MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( _capacityDest, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer );

            void * cache_buffer = buffer->getBuffer();

            size_t decompressSize;
            if( m_archivator->decompress( cache_buffer, _capacityDest, _source, _sourceSize, &decompressSize ) == false )
            {
                return 0;
            }

            uint8_t * buffer_ptr = static_cast<uint8_t *>(_dest);

            for( uint32_t j = 0; j != m_dataInfo.height; ++j )
            {
                read_byte += m_stream->read( buffer_ptr, m_dataInfo.width * channels );

                buffer_ptr += _pitch;
            }
        }

        return read_byte;
    }
    //////////////////////////////////////////////////////////////////////////
}

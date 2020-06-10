#include "ImageDecoderArchive.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

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
    size_t ImageDecoderArchive::_decode( void * const _buffer, size_t _bufferSize )
    {
        if( _bufferSize < m_options.pitch * m_dataInfo.height )
        {
            LOGGER_ERROR( "invalid bufferSize %zu != (%zu * %u)"
                , _bufferSize
                , m_options.pitch
                , m_dataInfo.height
            );

            return 0;
        }

        m_stream->seek( 0 );

        size_t read_byte = 0;

        if( m_dataInfo.channels == 4 && m_options.channels == 4 )
        {
            read_byte = this->decodeData_( _buffer, _bufferSize );
        }
        else if( m_dataInfo.channels == 3 && m_options.channels == 4 )
        {
            read_byte = this->decodeData_( _buffer, _bufferSize );

            this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, _buffer, m_options.pitch );
        }
        else
        {
            return 0;
        }

        return read_byte;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderArchive::decodeData_( void * const _buffer, size_t _bufferSize ) const
    {
        MENGINE_UNUSED( _bufferSize );

        size_t decodyByte = 0;

        size_t stream_size;
        void * stream_memory;
        if( m_stream->memory( &stream_memory, &stream_size ) == false )
        {
            MemoryInterfacePtr buffer = Helper::createMemoryCacheStream( m_stream, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, 0 );

            const void * cache_buffer = buffer->getBuffer();
            size_t cache_size = buffer->getSize();

            decodyByte = this->decompressData_( cache_buffer, cache_size, _buffer, m_uncompressSize );
        }
        else
        {
            decodyByte = this->decompressData_( stream_memory, stream_size, _buffer, m_uncompressSize );
        }

        return decodyByte;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderArchive::decompressData_( const void * _source, size_t _sourceSize, void * const _dest, size_t _capacityDest ) const
    {
        size_t read_byte = 0;

        if( m_options.pitch == m_dataInfo.width * m_dataInfo.channels )
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

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, 0 );

            void * cache_buffer = buffer->getBuffer();

            size_t decompressSize;
            if( m_archivator->decompress( cache_buffer, _capacityDest, _source, _sourceSize, &decompressSize ) == false )
            {
                return 0;
            }

            uint8_t * buffer_ptr = static_cast<uint8_t *>(_dest);
            for( uint32_t j = 0; j != m_dataInfo.height; ++j )
            {
                read_byte += m_stream->read( buffer_ptr, m_dataInfo.width * m_dataInfo.channels );

                buffer_ptr += m_options.pitch;
            }
        }

        return read_byte;
    }
}

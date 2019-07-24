#include "ImageDecoderWEBP.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderWEBP::ImageDecoderWEBP()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderWEBP::~ImageDecoderWEBP()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderWEBP::_prepareData()
    {
        VP8StatusCode status;
        WebPBitstreamFeatures features;

        size_t featuresBufferSize = 0;

        void * streamMemory;
        size_t streamSize;
        if( m_stream->memory( &streamMemory, &streamSize ) == false )
        {
            do
            {
                featuresBufferSize += 4096;

                MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( featuresBufferSize, MENGINE_DOCUMENT_FUNCTION );

                MENGINE_ASSERTION_MEMORY_PANIC( buffer, false );

                uint8_t * featuresMemory = buffer->getBuffer();

                MENGINE_ASSERTION_MEMORY_PANIC( featuresMemory, false, "invalid get memory %d"
                    , featuresBufferSize
                );

                m_stream->read( featuresMemory, featuresBufferSize );

                status = WebPGetFeatures( featuresMemory, featuresBufferSize, &features );

                m_stream->seek( 0 );
            } while( status == VP8_STATUS_NOT_ENOUGH_DATA );
        }
        else
        {
            const uint8_t * featuresMemory = static_cast<const uint8_t *>(streamMemory);

            status = WebPGetFeatures( featuresMemory, streamSize, &features );
        }

        if( status != VP8_STATUS_OK )
        {
            LOGGER_ERROR( "invalid WebPGetFeatures %d"
                , status
            );

            return false;
        }

        uint32_t channels;
        if( features.has_alpha == 0 )
        {
            channels = 3;
        }
        else
        {
            channels = 4;
        }

        //fill ImageCodecDataInfo strucuture
        m_dataInfo.width = features.width;
        m_dataInfo.height = features.height;
        m_dataInfo.depth = 1;

        m_dataInfo.mipmaps = 1;
        m_dataInfo.channels = channels;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderWEBP::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoderWEBP::_finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderWEBP::_decode( void * _buffer, size_t _bufferSize )
    {
        uint8_t * webp_buffer = static_cast<uint8_t *>(_buffer);
        size_t webp_buffer_size = _bufferSize;

        void * streamMemory;
        size_t streamSize;
        if( m_stream->memory( &streamMemory, &streamSize ) == false )
        {
            MemoryInterfacePtr buffer = Helper::createMemoryCacheStream( m_stream, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, 0, "invalid create memory for stream" );

            const uint8_t * buffer_memory = buffer->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( buffer_memory, 0 );

            size_t buffer_size = buffer->getSize();

            if( this->decodeWEBP_( buffer_memory, buffer_size, webp_buffer, webp_buffer_size ) == false )
            {
                return 0;
            }
        }
        else
        {
            const uint8_t * webp_source = static_cast<const uint8_t *>(streamMemory);
            size_t webp_source_size = streamSize;

            if( this->decodeWEBP_( webp_source, webp_source_size, webp_buffer, webp_buffer_size ) == false )
            {
                return 0;
            }
        }

        return _bufferSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderWEBP::decodeWEBP_( const uint8_t * _source, size_t _sourceSize, uint8_t * _buffer, size_t _bufferSize )
    {
#ifdef MENGINE_RENDER_TEXTURE_RGBA
#	define MENGINE_WEBP_DECODE_RGBA WebPDecodeRGBAInto
#	define MENGINE_WEBP_DECODE_RGB WebPDecodeRGBInto
#else
#	define MENGINE_WEBP_DECODE_RGBA WebPDecodeBGRAInto
#	define MENGINE_WEBP_DECODE_RGB WebPDecodeBGRInto
#endif

        if( m_dataInfo.channels == 4 && m_options.channels == 4 )
        {
            if( MENGINE_WEBP_DECODE_RGBA( _source, _sourceSize, _buffer, _bufferSize, (int32_t)m_options.pitch ) == nullptr )
            {
                return false;
            }
        }
        else if( m_dataInfo.channels == 3 && m_options.channels == 4 )
        {
            if( MENGINE_WEBP_DECODE_RGBA( _source, _sourceSize, _buffer, _bufferSize, (int32_t)m_options.pitch ) == nullptr )
            {
                return false;
            }
        }
        else if( m_dataInfo.channels == 3 && m_options.channels == 3 )
        {
            if( MENGINE_WEBP_DECODE_RGB( _source, _sourceSize, _buffer, _bufferSize, (int32_t)m_options.pitch ) == nullptr )
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        return true;
    }
}

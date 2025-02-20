#include "ImageDecoderWebP.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_RENDER_TEXTURE_RGBA)
#   define MENGINE_WEBP_DECODE_RGBA WebPDecodeRGBAInto
#   define MENGINE_WEBP_DECODE_RGB WebPDecodeRGBInto
#else
#   define MENGINE_WEBP_DECODE_RGBA WebPDecodeBGRAInto
#   define MENGINE_WEBP_DECODE_RGB WebPDecodeBGRInto
#endif
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

        const InputStreamInterfacePtr & stream = this->getStream();

        void * streamMemory;
        size_t streamSize;
        if( stream->memory( &streamMemory, &streamSize ) == false )
        {
            do
            {
                featuresBufferSize += 4096;

                MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( featuresBufferSize, MENGINE_DOCUMENT_FACTORABLE );

                MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create memory cache buffer" );

                uint8_t * featuresMemory = buffer->getBuffer();

                MENGINE_ASSERTION_MEMORY_PANIC( featuresMemory, "invalid get memory %zu"
                    , featuresBufferSize
                );

                size_t featuresBufferSizeRead = stream->read( featuresMemory, featuresBufferSize );

                status = WebPGetFeatures( featuresMemory, featuresBufferSizeRead, &features );

                stream->rewind();
            } while( status == VP8_STATUS_NOT_ENOUGH_DATA );
        }
        else
        {
            const uint8_t * featuresMemory = static_cast<const uint8_t *>(streamMemory);

            status = WebPGetFeatures( featuresMemory, streamSize, &features );
        }

        if( status != VP8_STATUS_OK )
        {
            LOGGER_ERROR( "image decoder WebP invalid WebPGetFeatures [%u]"
                , status
            );

            return false;
        }

        if( features.has_alpha == 0 )
        {
            m_dataInfo.format = PF_R8G8B8;
        }
        else
        {
            m_dataInfo.format = PF_A8R8G8B8;
        }

        //fill ImageCodecDataInfo strucuture
        m_dataInfo.width = features.width;
        m_dataInfo.height = features.height;

        m_dataInfo.mipmaps = 1;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderWEBP::_initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoderWEBP::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderWEBP::_decode( const DecoderData * _decoderData )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _decoderData, "invalid decode data" );
        MENGINE_ASSERTION_TYPE( _decoderData, const ImageDecoderData *, "invalid decode data" );

        MENGINE_PROFILER_CATEGORY();

        const ImageDecoderData * decoderData = static_cast<const ImageDecoderData *>(_decoderData);

        const InputStreamInterfacePtr & stream = this->getStream();

        void * streamMemory;
        size_t streamSize;
        if( stream->memory( &streamMemory, &streamSize ) == false )
        {
            MemoryInterfacePtr buffer = Helper::createMemoryCacheStream( stream, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create memory for stream" );

            const uint8_t * buffer_memory = buffer->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( buffer_memory, "invalid get memory" );

            size_t buffer_size = buffer->getSize();

            if( this->decodeWEBP_( buffer_memory, buffer_size, decoderData ) == false )
            {
                return 0;
            }
        }
        else
        {
            const uint8_t * webp_source = static_cast<const uint8_t *>(streamMemory);
            size_t webp_source_size = streamSize;

            if( this->decodeWEBP_( webp_source, webp_source_size, decoderData ) == false )
            {
                return 0;
            }
        }

        size_t size = decoderData->size;

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderWEBP::decodeWEBP_( const uint8_t * _source, size_t _sourceSize, const ImageDecoderData * _data )
    {
        uint8_t * const webp_buffer = static_cast<uint8_t * const>(_data->buffer);
        size_t webp_buffer_size = _data->size;
        size_t webp_buffer_pitch = _data->pitch;

        uint32_t optionChannels = Helper::getPixelFormatChannels( _data->format );
        uint32_t dataChannels = Helper::getPixelFormatChannels( m_dataInfo.format );

        if( dataChannels == 4 && optionChannels == 4 )
        {
            if( MENGINE_WEBP_DECODE_RGBA( _source, _sourceSize, webp_buffer, webp_buffer_size, (int32_t)webp_buffer_pitch ) == nullptr )
            {
                return false;
            }
        }
        else if( dataChannels == 3 && optionChannels == 4 )
        {
            if( MENGINE_WEBP_DECODE_RGBA( _source, _sourceSize, webp_buffer, webp_buffer_size, (int32_t)webp_buffer_pitch ) == nullptr )
            {
                return false;
            }
        }
        else if( dataChannels == 3 && optionChannels == 3 )
        {
            if( MENGINE_WEBP_DECODE_RGB( _source, _sourceSize, webp_buffer, webp_buffer_size, (int32_t)webp_buffer_pitch ) == nullptr )
            {
                return false;
            }
        }
        else
        {
            LOGGER_ERROR( "image decoder WebP not support for in %u and out %u channels"
                , dataChannels
                , optionChannels
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}

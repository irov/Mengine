#include "ImageDecoderMemory.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderMemory::ImageDecoderMemory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderMemory::~ImageDecoderMemory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderMemory::_decode( const DecoderData * _data )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _data );
        MENGINE_ASSERTION_TYPE( _data, const ImageDecoderData * );

        const ImageDecoderData * imageData = static_cast<const ImageDecoderData *>(_data);

        void * buffer = imageData->buffer;
        size_t size = imageData->size;

        MENGINE_ASSERTION_FATAL( imageData->size >= imageData->pitch * m_dataInfo.height, "invalid bufferSize %zu != (%zu * %u)"
            , imageData->size
            , imageData->pitch
            , m_dataInfo.height
        );

        const InputStreamInterfacePtr & stream = this->getStream();
        stream->seek( 0 );

        size_t read_byte = 0;

        uint32_t imageChannels = Helper::getPixelFormatChannels( m_dataInfo.format );
        uint32_t optionChannels = Helper::getPixelFormatChannels( imageData->format );

        if( imageChannels == 3 && optionChannels == 4 )
        {
            read_byte = this->decodeData_( buffer, size, imageData->pitch );

            this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, buffer, imageData->pitch );
        }
        else if( imageChannels == 4 && optionChannels == 4 )
        {
            read_byte = this->decodeData_( buffer, size, imageData->pitch );
        }
        else
        {
            LOGGER_ERROR( "invalid channels data %u options %u"
                , imageChannels
                , optionChannels
            );

            return 0;
        }

        return read_byte;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderMemory::decodeData_( void * const _buffer, size_t _bufferSize, size_t _pitch ) const
    {
        size_t read_byte = 0;

        const InputStreamInterfacePtr & stream = this->getStream();

        uint32_t channels = Helper::getPixelFormatChannels( m_dataInfo.format );

        if( _pitch == m_dataInfo.width * channels )
        {
            uint8_t * buffer_ptr = static_cast<uint8_t *>(_buffer);

            read_byte += stream->read( buffer_ptr, _bufferSize );
        }
        else
        {
            uint8_t * buffer_ptr = static_cast<uint8_t *>(_buffer);

            for( uint32_t j = 0; j != m_dataInfo.height; ++j )
            {
                read_byte += stream->read( buffer_ptr, m_dataInfo.width * channels );

                buffer_ptr += _pitch;
            }
        }

        return read_byte;
    }
    //////////////////////////////////////////////////////////////////////////
}

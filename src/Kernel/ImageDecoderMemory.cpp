#include "ImageDecoderMemory.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"

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
    size_t ImageDecoderMemory::_decode( void * const _buffer, size_t _bufferSize )
    {
        MENGINE_ASSERTION_FATAL( _bufferSize >= m_options.pitch * m_dataInfo.height, "invalid bufferSize %zu != (%zu * %u)"
            , _bufferSize
            , m_options.pitch
            , m_dataInfo.height
        );

        m_stream->seek( 0 );

        size_t read_byte = 0;

        if( m_dataInfo.channels == 3 && m_options.channels == 4 )
        {
            read_byte = this->decodeData_( _buffer, _bufferSize );

            this->sweezleAlpha3( m_dataInfo.width, m_dataInfo.height, _buffer, m_options.pitch );
        }
        else if( m_dataInfo.channels == 4 && m_options.channels == 4 )
        {
            read_byte = this->decodeData_( _buffer, _bufferSize );
        }
        else
        {
            MENGINE_ERROR_FATAL( "invalid channels data %u options %u"
                , m_dataInfo.channels 
                , m_options.channels
            );
        }

        return read_byte;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderMemory::decodeData_( void * const _buffer, size_t _bufferSize ) const
    {
        size_t read_byte = 0;

        if( m_options.pitch == m_dataInfo.width * m_dataInfo.channels )
        {
            uint8_t * buffer_ptr = static_cast<uint8_t *>(_buffer);

            read_byte += m_stream->read( buffer_ptr, _bufferSize );
        }
        else
        {
            uint8_t * buffer_ptr = static_cast<uint8_t *>(_buffer);

            for( uint32_t j = 0; j != m_dataInfo.height; ++j )
            {
                read_byte += m_stream->read( buffer_ptr, m_dataInfo.width * m_dataInfo.channels );

                buffer_ptr += m_options.pitch;
            }
        }

        return read_byte;
    }
}

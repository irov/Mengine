#include "ImageDecoder.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImageDecoder::ImageDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoder::~ImageDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoder::setOptions( const CodecOptions * _options )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _options, false );
        MENGINE_ASSERTION_TYPE( _options, const ImageCodecOptions * );

        m_options = *static_cast<const ImageCodecOptions *>(_options);

        MENGINE_ASSERTION_FATAL_RETURN( m_options.pitch != 0, false, "pitch == 0" );
        MENGINE_ASSERTION_FATAL_RETURN( m_options.channels != 0, false, "pitch == 0" );

        bool result = this->_invalidateOptions();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoder::_invalidateOptions()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ImageCodecDataInfo * ImageDecoder::getCodecDataInfo() const
    {
        return &m_dataInfo;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoder::setCodecDataInfo( const CodecDataInfo * _dataInfo )
    {
        MENGINE_ASSERTION_MEMORY_PANIC_VOID( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const ImageCodecDataInfo * );

        m_dataInfo = *static_cast<const ImageCodecDataInfo *>(_dataInfo);
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoder::sweezleAlpha1( uint32_t _width, uint32_t _height, void * _buffer, size_t _pitch )
    {
        uint8_t * sweezle_buffer = static_cast<uint8_t *>(_buffer);

        for( uint32_t h = _height; h != 0; --h )
        {
            size_t hp = (h - 1) * _pitch;

            for( uint32_t w = _width; w != 0; --w )
            {
                sweezle_buffer[hp + (w - 1) * 4 + 3] = sweezle_buffer[hp + (w - 1)];
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoder::sweezleAlpha3( uint32_t _width, uint32_t _height, void * _buffer, size_t _pitch )
    {
        uint8_t * sweezle_buffer = static_cast<uint8_t *>(_buffer);

        for( uint32_t h = _height; h != 0; --h )
        {
            size_t hp = (h - 1) * _pitch;

            for( uint32_t w = _width; w != 0; --w )
            {
                uint8_t * sweezle_buffer_rgba = sweezle_buffer + hp + (w - 1) * 4;
                uint8_t * sweezle_buffer_rgb = sweezle_buffer + hp + (w - 1) * 3;

                sweezle_buffer_rgba[0] = sweezle_buffer_rgb[0];
                sweezle_buffer_rgba[1] = sweezle_buffer_rgb[1];
                sweezle_buffer_rgba[2] = sweezle_buffer_rgb[2];
                sweezle_buffer_rgba[3] = 255;
            }
        }
    }
}

#include "TTFFontTextureGlyphProvider.h"

#include "Kernel/MemoryCopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFFontTextureGlyphProvider::TTFFontTextureGlyphProvider( uint32_t _width, uint32_t _height, const void * _ttfbuffer, size_t _ttfpitch, uint32_t _ttfchannel )
        : m_width( _width )
        , m_height( _height )
        , m_ttfbuffer( _ttfbuffer )
        , m_ttfpitch( _ttfpitch )
        , m_ttfchannel( _ttfchannel )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFFontTextureGlyphProvider::~TTFFontTextureGlyphProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFontTextureGlyphProvider::onTextureGlyphFill( uint8_t * const _memory, size_t _pitch, uint32_t _channel ) const
    {
        const uint8_t * glyph_buffer = reinterpret_cast<const uint8_t *>(m_ttfbuffer);

        uint32_t border_width = m_width + 2;
        uint32_t border_height = m_height + 2;

        uint8_t * it_texture_memory = _memory;
        const uint8_t * it_glyph_buffer = glyph_buffer;

        it_texture_memory += _pitch; //y offset one pixel
        it_texture_memory += _channel; //x offset one pixel

        if( m_ttfchannel == 1 && _channel == 4 )
        {
            for( uint32_t h = 0; h != m_height; ++h )
            {
                for( uint32_t w = 0; w != m_width; ++w )
                {
                    uint8_t * it_texture_memory_place = it_texture_memory + w * 4;

                    const uint8_t glyph_pixel = *(it_glyph_buffer + w);

                    *it_texture_memory_place++ = 255;
                    *it_texture_memory_place++ = 255;
                    *it_texture_memory_place++ = 255;
                    *it_texture_memory_place++ = glyph_pixel;
                }

                it_texture_memory += _pitch;
                it_glyph_buffer += m_ttfpitch;
            }
        }
        else if( m_ttfchannel == 4 && _channel == 4 )
        {
            for( uint32_t h = 0; h != m_height; ++h )
            {
                Helper::memoryCopyPod( it_texture_memory, 0, it_glyph_buffer, m_width * m_ttfchannel );

                it_texture_memory += _pitch;
                it_glyph_buffer += m_ttfpitch;
            }
        }
        else
        {
            return false;
        }

        uint8_t * it_top_border_texture_memory = _memory;
        for( uint32_t w = 0; w != border_width; ++w )
        {
            uint8_t * it_texture_memory_place = it_top_border_texture_memory;

            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;

            it_top_border_texture_memory += _channel;
        }

        uint8_t * it_bottom_border_texture_memory = _memory + _pitch * border_height - _pitch;
        for( uint32_t w = 0; w != border_width; ++w )
        {
            uint8_t * it_texture_memory_place = it_bottom_border_texture_memory;

            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;

            it_bottom_border_texture_memory += _channel;
        }

        uint8_t * it_left_border_texture_memory = _memory + _pitch;
        for( uint32_t h = 0; h != m_height; ++h )
        {
            uint8_t * it_texture_memory_place = it_left_border_texture_memory;

            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;

            it_left_border_texture_memory += _pitch;
        }

        uint8_t * it_right_border_texture_memory = _memory + _pitch + _channel + m_width * _channel;
        for( uint32_t h = 0; h != m_height; ++h )
        {
            uint8_t * it_texture_memory_place = it_right_border_texture_memory;

            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;
            *it_texture_memory_place++ = 0;

            it_right_border_texture_memory += _pitch;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
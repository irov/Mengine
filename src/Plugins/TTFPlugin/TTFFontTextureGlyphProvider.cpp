#include "TTFFontTextureGlyphProvider.h"

#include "Kernel/MemoryCopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFFontTextureGlyphProvider::TTFFontTextureGlyphProvider( uint32_t _width, uint32_t _height, const void * _ttfmemory, size_t _ttfpitch, uint32_t _ttfchannel )
        : m_width( _width )
        , m_height( _height )
        , m_ttfmemory( _ttfmemory )
        , m_ttfpitch( _ttfpitch )
        , m_ttfchannel( _ttfchannel )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFFontTextureGlyphProvider::~TTFFontTextureGlyphProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFontTextureGlyphProvider::onTextureGlyphFill( uint8_t * const _memory, size_t _pitch, uint32_t _channel, uint32_t _border ) const
    {
        const uint8_t * glyph_memory = reinterpret_cast<const uint8_t *>(m_ttfmemory);

        uint8_t * it_texture_memory = _memory;
        const uint8_t * it_glyph_memory = glyph_memory;

        it_texture_memory += _border * _pitch; //y offset one pixel
        it_texture_memory += _border * _channel; //x offset one pixel

        if( m_ttfchannel == 1 && _channel == 4 )
        {
            for( uint32_t h = 0; h != m_height; ++h )
            {
                uint8_t * it_texture_memory_place = it_texture_memory;

                for( uint32_t w = 0; w != m_width; ++w )
                {
                    *it_texture_memory_place++ = 255;
                    *it_texture_memory_place++ = 255;
                    *it_texture_memory_place++ = 255;
                    *it_texture_memory_place++ = *it_glyph_memory++;
                }

                it_texture_memory += _pitch;
            }
        }
        else if( m_ttfchannel == 4 && _channel == 4 )
        {
            for( uint32_t h = 0; h != m_height; ++h )
            {
                uint8_t * it_texture_memory_place = it_texture_memory;

                for( uint32_t w = 0; w != m_width; ++w )
                {
                    *it_texture_memory_place++ = *it_glyph_memory++;
                    *it_texture_memory_place++ = *it_glyph_memory++;
                    *it_texture_memory_place++ = *it_glyph_memory++;
                    *it_texture_memory_place++ = *it_glyph_memory++;
                }

                it_texture_memory += _pitch;
            }
        }
        else
        {
            return false;
        }

        uint32_t border_width = m_width + _border * 2;
        uint32_t border_height = m_height + _border * 2;

        uint8_t * it_top_border_texture_memory = _memory;
        uint8_t * it_top_border_texture_memory_copy = _memory + _pitch;

        for( uint32_t w = 0; w != border_width; ++w )
        {
            uint8_t * it_texture_memory_place = it_top_border_texture_memory;
            uint8_t * it_texture_memory_copy_place = it_top_border_texture_memory_copy;

            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;

            it_top_border_texture_memory += _channel;
            it_top_border_texture_memory_copy += _channel;
        }

        uint8_t * it_bottom_border_texture_memory = _memory + _pitch * border_height - _pitch;
        uint8_t * it_bottom_border_texture_memory_copy = _memory + _pitch * border_height - _pitch * 2;

        for( uint32_t w = 0; w != border_width; ++w )
        {
            uint8_t * it_texture_memory_place = it_bottom_border_texture_memory;
            uint8_t * it_texture_memory_copy_place = it_bottom_border_texture_memory_copy;

            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;

            it_bottom_border_texture_memory += _channel;
            it_bottom_border_texture_memory_copy += _channel;
        }

        uint8_t * it_left_border_texture_memory = _memory;
        uint8_t * it_left_border_texture_memory_copy = _memory + _channel;

        for( uint32_t h = 0; h != border_height; ++h )
        {
            uint8_t * it_texture_memory_place = it_left_border_texture_memory;
            uint8_t * it_texture_memory_copy_place = it_left_border_texture_memory_copy;

            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;

            it_left_border_texture_memory += _pitch;
            it_left_border_texture_memory_copy += _pitch;
        }

        uint8_t * it_right_border_texture_memory = _memory + border_width * _channel - _channel;
        uint8_t * it_right_border_texture_memory_copy = _memory + border_width * _channel - _channel * 2;

        for( uint32_t h = 0; h != border_height; ++h )
        {
            uint8_t * it_texture_memory_place = it_right_border_texture_memory;
            uint8_t * it_texture_memory_copy_place = it_right_border_texture_memory_copy;

            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;
            *it_texture_memory_place++ = *it_texture_memory_copy_place++;

            it_right_border_texture_memory += _pitch;
            it_right_border_texture_memory_copy += _pitch;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
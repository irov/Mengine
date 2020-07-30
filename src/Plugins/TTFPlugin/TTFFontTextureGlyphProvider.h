#pragma once

#include "TTFInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class TTFFontTextureGlyphProvider
        : public TTFTextureGlyphProviderInterface
        , public Factorable
    {
    public:
        TTFFontTextureGlyphProvider( uint32_t _width, uint32_t _height, const void * _ttfbuffer, size_t _ttfpitch, uint32_t _ttfchannel );
        ~TTFFontTextureGlyphProvider() override;

    public:
        bool onTextureGlyphFill( uint8_t * const _memory, size_t _pitch, uint32_t _channel ) override;

    protected:
        uint32_t m_width;
        uint32_t m_height;

        const void * m_ttfbuffer;
        size_t m_ttfpitch;
        uint32_t m_ttfchannel;
    };
}
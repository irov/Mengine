#pragma once

#include "Kernel/ImageDecoder.h"

namespace Mengine
{
    class ImageDecoderETC1
        : public ImageDecoder
    {
    public:
        ImageDecoderETC1();
        ~ImageDecoderETC1() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( void * _buffer, size_t _bufferSize ) override;

    protected:
        struct ETC1Header
        {
            char tag[6];	       // "PKM 10"
            uint16_t format;         // Format == number of mips (== zero)
            uint16_t texWidth;       // Texture dimensions, multiple of 4 (big-endian)
            uint16_t texHeight;
            uint16_t origWidth;      // Original dimensions (big-endian)
            uint16_t origHeight;
        };

        ETC1Header m_etc1_ptr;
        bool m_needSeek;
    };
}
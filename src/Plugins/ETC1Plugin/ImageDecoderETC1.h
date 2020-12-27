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
        size_t _decode( void * const _buffer, size_t _bufferSize ) override;

    protected:
        struct ETC1Header
        {
            Char tag[6];
            uint16_t format;
            uint16_t texWidth;
            uint16_t texHeight;
            uint16_t origWidth;
            uint16_t origHeight;
        };

        ETC1Header m_header = {};
    };
}
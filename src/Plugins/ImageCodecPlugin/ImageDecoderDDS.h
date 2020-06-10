#pragma once

#include "Kernel/ImageDecoder.h"

namespace Mengine
{
    class ImageDecoderDDS
        : public ImageDecoder
    {
    public:
        ImageDecoderDDS();
        ~ImageDecoderDDS() override;

    public:
        bool _initialize() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( void * const _buffer, size_t _bufferSize ) override;
    };
}

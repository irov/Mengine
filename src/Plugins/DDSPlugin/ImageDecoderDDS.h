#pragma once

#include "Kernel/ImageDecoder.h"

namespace Mengine
{
    class ImageDecoderDDS
        : public ImageDecoder
    {
        DECLARE_FACTORABLE( ImageDecoderDDS );

    public:
        ImageDecoderDDS();
        ~ImageDecoderDDS() override;

    public:
        bool _initialize() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( const DecoderData * _data ) override;
    };
}

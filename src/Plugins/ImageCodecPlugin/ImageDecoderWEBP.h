#pragma once

#include "Kernel/ImageDecoder.h"

#include "webp/decode.h"
#include "webp/mux.h"

namespace Mengine
{
    class ImageDecoderWEBP
        : public ImageDecoder
    {
    public:
        ImageDecoderWEBP();
        ~ImageDecoderWEBP() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( void * _buffer, size_t _bufferSize ) override;

    protected:
        bool decodeWEBP_( const uint8_t * _source, size_t _sourceSize, uint8_t * _buffer, size_t _bufferSize );
    };
}
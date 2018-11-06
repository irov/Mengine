#pragma once

#include "Kernel/ImageEncoder.h"

#include "png.h"

namespace Mengine
{
    class ImageEncoderPNG
        : public ImageEncoder
    {
    public:
        ImageEncoderPNG();
        ~ImageEncoderPNG() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        size_t encode( const void * _buffer, size_t _size, const CodecDataInfo* _bufferDataInfo ) override;

    private:
        png_structp m_png_ptr;
        png_infop m_info_ptr;
    };
}

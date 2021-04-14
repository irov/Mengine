#pragma once

#include "Kernel/ImageEncoder.h"

namespace Mengine
{
    class ImageEncoderDDS
        : public ImageEncoder
    {
        DECLARE_FACTORABLE( ImageEncoderDDS );

    public:
        ImageEncoderDDS();
        ~ImageEncoderDDS() override;

    public:
        bool _initialize() override;

    public:
        size_t encode( const void * _buffer, size_t _size, const CodecDataInfo * _dataInfo ) override;
    };
}

#pragma once

#include "JPEGIncluder.h"

#include "Kernel/ImageEncoder.h"

namespace Mengine
{
    class ImageEncoderJPEG
        : public ImageEncoder
    {
        DECLARE_FACTORABLE( ImageEncoderJPEG );

    public:
        ImageEncoderJPEG();
        ~ImageEncoderJPEG() override;

    public:
        bool _initialize() override;

    public:
        size_t encode( const EncoderData * _encoderData, const CodecDataInfo * _dataInfo ) override;
    };
}

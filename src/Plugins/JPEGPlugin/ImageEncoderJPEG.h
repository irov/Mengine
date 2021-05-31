#pragma once

#include "Kernel/ImageEncoder.h"

extern "C"
{
#define XMD_H
#undef FAR

#include <csetjmp>
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
}

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

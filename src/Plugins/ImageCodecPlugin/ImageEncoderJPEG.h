#pragma once

#include "Interface/ImageCodecInterface.h"

#include "Kernel/ImageEncoder.h"

extern "C"
{
#define XMD_H
#undef FAR

#include <setjmp.h>
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
}

namespace Mengine
{
    class ImageEncoderJPEG
        : public ImageEncoder
    {
    public:
        ImageEncoderJPEG();
        ~ImageEncoderJPEG() override;

    public:
        bool _initialize() override;

    public:
        size_t encode( const void * _buffer, size_t _size, const CodecDataInfo * _bufferDataInfo ) override;
    };
}

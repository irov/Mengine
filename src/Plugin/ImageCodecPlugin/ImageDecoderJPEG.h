#pragma once

#include "Kernel/ImageDecoder.h"

extern "C"
{
#	define XMD_H
#	undef FAR

#include <setjmp.h>
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
#	define JPEG_INTERNALS
#include "jmorecfg.h"
}

namespace Mengine
{
    class ImageDecoderJPEG
        : public ImageDecoder
    {
    public:
        ImageDecoderJPEG();
        ~ImageDecoderJPEG() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        bool _prepareData() override;

    protected:
        size_t _decode( void * _buffer, size_t _bufferSize ) override;

    protected:
        bool _rewind() override;

    private:
        jpeg_decompress_struct m_jpegObject;
        jpeg_error_mgr m_errorMgr;
    };
}

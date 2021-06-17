#pragma once

#include "JPEGIncluder.h"

#include "Kernel/ImageDecoder.h"

namespace Mengine
{
    class ImageDecoderJPEG
        : public ImageDecoder
    {
        DECLARE_FACTORABLE( ImageDecoderJPEG );

    public:
        ImageDecoderJPEG();
        ~ImageDecoderJPEG() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        bool _prepareData() override;

    protected:
        size_t _decode( const DecoderData * _data ) override;

    protected:
        bool _rewind() override;

    private:
        jpeg_decompress_struct m_jpegObject;
        jpeg_error_mgr m_errorMgr;
    };
}

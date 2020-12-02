#pragma once

#include "Kernel/ImageDecoder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ImageDecoderMemory
        : public ImageDecoder
    {
    public:
        ImageDecoderMemory();
        ~ImageDecoderMemory() override;

    protected:
        size_t _decode( void * const _buffer, size_t _bufferSize ) override;

    protected:
        size_t decodeData_( void * const _buffer, size_t _bufferSize ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImageDecoderMemory> ImageDecoderMemoryPtr;
    //////////////////////////////////////////////////////////////////////////
}

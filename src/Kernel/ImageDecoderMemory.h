#pragma once

#include "Kernel/ImageDecoder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ImageDecoderMemory
        : public ImageDecoder
    {
        DECLARE_FACTORABLE( ImageDecoderMemory );

    public:
        ImageDecoderMemory();
        ~ImageDecoderMemory() override;

    protected:
        size_t _decode( const DecoderData * _data ) override;

    protected:
        size_t decodeData_( void * const _buffer, size_t _bufferSize, size_t _pitch ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImageDecoderMemory> ImageDecoderMemoryPtr;
    //////////////////////////////////////////////////////////////////////////
}

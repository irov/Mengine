#pragma once

#include "Interface/ImageCodecInterface.h"

#include "Kernel/Encoder.h"

namespace Mengine
{
    class ImageEncoder
        : public Encoder<ImageEncoderInterface>
    {
    public:
        ImageEncoder();
        ~ImageEncoder() override;

    public:
        bool setOptions( const CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    protected:
        ImageCodecOptions m_options;
    };
    //////////////////////////////////////////////////////////////////////////
}

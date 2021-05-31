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
    };
}

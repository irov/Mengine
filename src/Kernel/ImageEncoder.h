#pragma once

#include "Interface/ImageCodecInterface.h"

#include "Kernel/EncoderBase.h"

namespace Mengine
{
    class ImageEncoder
        : public EncoderBase<ImageEncoderInterface>
    {
    public:
        ImageEncoder();
        ~ImageEncoder() override;
    };
}

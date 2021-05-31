#pragma once

#include "Interface/PickCodecInterface.h"

#include "Kernel/Encoder.h"

namespace Mengine
{
    class PickEncoder
        : public Encoder<PickEncoderInterface>
    {
    public:
        PickEncoder();
        ~PickEncoder() override;
    };
}

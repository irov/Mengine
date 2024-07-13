#pragma once

#include "Interface/PickCodecInterface.h"

#include "Kernel/EncoderBase.h"

namespace Mengine
{
    class PickEncoder
        : public EncoderBase<PickEncoderInterface>
    {
    public:
        PickEncoder();
        ~PickEncoder() override;
    };
}

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

    public:
        bool setOptions( const CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    protected:
        PickCodecOptions m_options;
    };
}

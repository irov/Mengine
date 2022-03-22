#pragma once

#include "Interface/DecoderInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct SoundCodecDataInfo
        : public CodecDataInfo
    {
        size_t size = 0;
        uint32_t frequency = 0;
        uint32_t channels = 0;
        uint32_t bits = 0;

        float length = 0.f;
    };
    //////////////////////////////////////////////////////////////////////////
    struct SoundDecoderData
        : public DecoderData
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class SoundDecoderInterface
        : public DecoderInterface
    {
    public:
        virtual const SoundCodecDataInfo * getCodecDataInfo() const override = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundDecoderInterface, DecoderInterface> SoundDecoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

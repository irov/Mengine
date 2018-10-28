#pragma once

#include "Interface/SoundCodecInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SoundBufferInterface
        : public Mixin
    {
    public:
        virtual bool update() = 0;

    public:
        virtual const SoundDecoderInterfacePtr & getDecoder() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundBufferInterface> SoundBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
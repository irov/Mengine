#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/SoundCodecInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SoundBufferInterface
        : public ServantInterface
    {
    public:
        virtual bool acquireSoundBuffer() = 0;
        virtual void releaseSoundBuffer() = 0;

    public:
        virtual bool updateSoundBuffer() = 0;

    public:
        virtual const SoundDecoderInterfacePtr & getDecoder() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundBufferInterface> SoundBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
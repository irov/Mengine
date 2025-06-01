#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/SoundCodecInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/UpdationInterface.h"
#include "Interface/MixerValueInterface.h"
#include "Interface/SoundBufferInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SoundSourceInterface
        : public Mixin
    {
    public:
        virtual bool play() = 0;
        virtual void stop() = 0;
        virtual void pause() = 0;
        virtual bool resume() = 0;

        virtual bool isPlay() const = 0;
        virtual bool isPause() const = 0;

        virtual void setVolume( float _volume ) = 0;
        virtual float getVolume() const = 0;

        virtual void setLoop( bool _loop ) = 0;
        virtual bool getLoop() const = 0;

        virtual float getDuration() const = 0;

        virtual bool setPosition( float _position ) = 0;
        virtual float getPosition() const = 0;

        virtual void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) = 0;
        virtual const SoundBufferInterfacePtr & getSoundBuffer() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundSourceInterface> SoundSourceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
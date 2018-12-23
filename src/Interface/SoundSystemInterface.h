#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/SoundCodecInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/UpdationInterface.h"
#include "Interface/SoundBufferInterface.h"
#include "Interface/SoundSourceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/MixerValue.h"

#include "math/vec3.h"

namespace Mengine
{    
    class SoundSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SoundSystem" )

    public:
        virtual void update() = 0;

    public:
        virtual bool isSilent() const = 0;

    public:
        virtual void onTurnSound( bool _turn ) = 0;

    public:
        virtual SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable ) = 0;

    public:
        virtual SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SOUND_SYSTEM()\
    ((Mengine::SoundSystemInterface *)SERVICE_GET(Mengine::SoundSystemInterface))
    //////////////////////////////////////////////////////////////////////////
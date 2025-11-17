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
#include "Kernel/Unknowable.h"

#include "math/vec3.h"

namespace Mengine
{
    class SoundSystemInterface
        : public ServiceInterface
        , public Unknowable
    {
        SERVICE_DECLARE( "SoundSystem" )

    public:
        virtual void update() = 0;

    public:
        virtual bool isSilent() const = 0;

    public:
        virtual SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _streamable, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, const DocumentInterfacePtr & _doc ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SOUND_SYSTEM()\
    ((Mengine::SoundSystemInterface *)SERVICE_GET(Mengine::SoundSystemInterface))
//////////////////////////////////////////////////////////////////////////
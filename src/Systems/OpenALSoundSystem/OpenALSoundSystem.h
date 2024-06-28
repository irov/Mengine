#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "OpenALSoundIncluder.h"
#include "OpenALSoundBufferBase.h"
#include "OpenALSoundBufferStream.h"
#include "OpenALSoundSource.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{    
    class OpenALSoundSystem
        : public ServiceBase<SoundSystemInterface>
    {
    public:
        OpenALSoundSystem();
        ~OpenALSoundSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void update() override;

    public:
        bool isSilent() const override;

    public:
        void onTurnSound( bool _turn ) override;

    public:
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, const DocumentInterfacePtr & _doc ) override;
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream, const DocumentInterfacePtr & _doc ) override;

    public:
        ALuint genSourceId();
        void releaseSourceId( ALuint _sourceId );

        ALuint genBufferId();
        void releaseBufferId( ALuint _bufferId );

    protected:
        void onDestroyOpenALSoundSource_( OpenALSoundSource * _soundSource );

    protected:
        ALCcontext * m_context;
        ALCdevice * m_device;

        FactoryInterfacePtr m_factoryOpenALSoundBuffer;
        FactoryInterfacePtr m_factoryOpenALSoundBufferStream;
        FactoryInterfacePtr m_factoryOpenALSoundSource;
    };    
}

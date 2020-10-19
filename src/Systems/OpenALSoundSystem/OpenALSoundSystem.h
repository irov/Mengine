#pragma once

#include "Interface/SoundSystemInterface.h"

#include "OpenALSoundBufferBase.h"
#include "OpenALSoundBufferStream.h"
#include "OpenALSoundSource.h"

#include "Kernel/Factory.h"

#include "Kernel/ServiceBase.h"

#include "AL/al.h"
#include "AL/alc.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
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
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, const DocumentPtr & _doc ) override;
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream, const DocumentPtr & _doc ) override;

    public:
        ALuint genSourceId();
        void releaseSourceId( ALuint _sourceId );

        ALuint genBufferId();
        void releaseBufferId( ALuint _bufferId );

    public:
        uint32_t getSourcesCount() const override;
        uint32_t getBuffersCount() const override;

    protected:
        void onDestroyOpenALSoundSource_( OpenALSoundSource * _soundSource );

    protected:
        ALCcontext * m_context;
        ALCdevice * m_device;

        uint32_t m_sourcesCount;
        uint32_t m_buffersCount;

        FactoryPtr m_factoryOpenALSoundBuffer;
        FactoryPtr m_factoryOpenALSoundBufferStream;
        FactoryPtr m_factoryOpenALSoundSource;
    };
    //////////////////////////////////////////////////////////////////////////
}

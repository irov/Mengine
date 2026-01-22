#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "OpenALSoundSystemExtensionInterface.h"

#include "OpenALSoundIncluder.h"
#include "OpenALSoundBufferBase.h"
#include "OpenALSoundBufferStream.h"
#include "OpenALSoundSource.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Unknowable.h"

namespace Mengine
{    
    class OpenALSoundSystem
        : public ServiceBase<SoundSystemInterface>
        , public OpenALSoundSystemExtensionInterface
    {
        DECLARE_FACTORABLE( OpenALSoundSystem );
        DECLARE_UNKNOWABLE();

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
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, const DocumentInterfacePtr & _doc ) override;
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _isStream, const DocumentInterfacePtr & _doc ) override;

    public:
        ALuint genSourceId() override;
        void releaseSourceId( ALuint _sourceId ) override;

        ALuint genBufferId() override;
        void releaseBufferId( ALuint _bufferId ) override;

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

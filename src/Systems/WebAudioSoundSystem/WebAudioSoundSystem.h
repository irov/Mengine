#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class WebAudioSoundSystem
        : public ServiceBase<SoundSystemInterface>
    {
    public:
        WebAudioSoundSystem();
        ~WebAudioSoundSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void update() override;
        bool isSilent() const override;
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _streamable, const DocumentInterfacePtr & _doc ) override;
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc ) override;

    protected:
        FactoryInterfacePtr m_factoryBuffer;
        FactoryInterfacePtr m_factorySource;
    };
}

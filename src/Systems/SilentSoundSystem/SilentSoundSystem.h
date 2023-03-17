#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "SilentSoundBuffer.h"
#include "SilentSoundSource.h"

#include "Kernel/ServiceBase.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    class SilentSoundSystem
        : public ServiceBase<SoundSystemInterface>
    {
    public:
        SilentSoundSystem();
        ~SilentSoundSystem() override;

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
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable, const DocumentPtr & _doc ) override;

    public:
        UniqueId genSourceId();
        void releaseSourceId( UniqueId _sourceId );
        void clearSourceId( UniqueId _sourceId );

    public:
        UniqueId genBufferId();
        void releaseBufferId( UniqueId _bufferId );

    public:
        uint32_t getSourcesCount() const override;
        uint32_t getBuffersCount() const override;

    protected:
        FactoryInterfacePtr m_factorySilentSoundBuffer;
        FactoryInterfacePtr m_factorySilentSoundSource;
    };
}

#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/Factory.h"

#include "SilentSoundBuffer.h"
#include "SilentSoundSource.h"

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
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample ) override;
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable ) override;

    public:
        uint32_t genSourceId();
        void releaseSourceId( uint32_t _sourceId );
        void clearSourceId( uint32_t _sourceId );

    public:
        uint32_t genBufferId();
        void releaseBufferId( uint32_t _bufferId );

    protected:
        FactoryPtr m_factorySilentSoundBuffer;
        FactoryPtr m_factorySilentSoundSource;
    };
}

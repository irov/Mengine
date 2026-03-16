#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factorable.h"
#include "Kernel/Unknowable.h"

namespace Mengine
{
    class IOSAVSoundSystem;
    class IOSAVSoundSource;
    class IOSAVSoundBuffer;

    typedef IntrusivePtr<IOSAVSoundBuffer, SoundBufferInterface> IOSAVSoundBufferPtr;
    typedef IntrusivePtr<IOSAVSoundSource, SoundSourceInterface> IOSAVSoundSourcePtr;

    class IOSAVSoundSystem
        : public ServiceBase<SoundSystemInterface>
    {
        DECLARE_UNKNOWABLE();

    public:
        IOSAVSoundSystem();
        ~IOSAVSoundSystem() override;

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
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _streamable, const DocumentInterfacePtr & _doc ) override;
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, const DocumentInterfacePtr & _doc ) override;

    public:
        void startEngine();
        void stopEngine();

    public:
        void setInterrupted( bool _interrupted );
        bool isInterrupted() const;

    public:
        void * getEngine() const;

    protected:
        bool m_engineStarted;
        bool m_interrupted;

        void * m_engine;

        FactoryInterfacePtr m_factorySoundBuffer;
        FactoryInterfacePtr m_factorySoundSource;
    };
}


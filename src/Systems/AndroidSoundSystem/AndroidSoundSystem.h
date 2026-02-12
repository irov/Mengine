#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/FactoryWithMutex.h"
#include "Kernel/ThreadMutex.h"
#include "Kernel/Vector.h"

#include "Config/Lambda.h"

#include <SLES/OpenSLES.h>

namespace Mengine
{
    class AndroidSoundBuffer;
    class AndroidSoundSource;

    class AndroidSoundSystem
        : public ServiceBase<SoundSystemInterface>
    {
    public:
        AndroidSoundSystem();
        ~AndroidSoundSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void _stopService() override;

    public:
        void update() override;

    public:
        bool isSilent() const override;
        void onTurnSound( bool _turn ) override;

    public:
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _streamable, const DocumentInterfacePtr & _doc ) override;
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, const DocumentInterfacePtr & _doc ) override;

    public:
        SLEngineItf getSLEngine() const;
        SLObjectItf getOutputMixObject() const;

    public:
        void registerSoundSource( AndroidSoundSource * _source );
        void unregisterSoundSource( AndroidSoundSource * _source );

    protected:
        void foreachSoundSource_( const Lambda<void( AndroidSoundSource * )> & _lambda );

    private:
        FactoryInterfacePtr m_factorySoundBuffer;
        FactoryInterfacePtr m_factorySoundSource;

        SLObjectItf m_engineObject;
        SLEngineItf m_engineEngine;
        SLObjectItf m_outputMixObject;

        bool m_soundEnabled;

        typedef Vector<AndroidSoundSource *> VectorAndroidSoundSources;
        VectorAndroidSoundSources m_sources;
        ThreadMutexInterfacePtr m_sourcesMutex;
    };
}


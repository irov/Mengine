#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "AndroidSoundSystemExtensionInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Unknowable.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidSoundSource;
    //////////////////////////////////////////////////////////////////////////
    class AndroidSoundSystem
        : public ServiceBase<SoundSystemInterface>
        , public AndroidSoundSystemExtensionInterface
    {
        DECLARE_FACTORABLE( AndroidSoundSystem );
        DECLARE_UNKNOWABLE();

    public:
        AndroidSoundSystem();
        ~AndroidSoundSystem() override;

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
        SLEngineItf getEngineInterface() const override;
        SLObjectItf getOutputMixObject() const override;

    protected:
        void onDestroyAndroidSoundSource_( AndroidSoundSource * _soundSource );

    protected:
        SLObjectItf m_engineObject;
        SLEngineItf m_engineItf;
        SLObjectItf m_outputMixObject;

        FactoryInterfacePtr m_factoryAndroidSoundBuffer;
        FactoryInterfacePtr m_factoryAndroidSoundBufferStream;
        FactoryInterfacePtr m_factoryAndroidSoundSource;
    };
}

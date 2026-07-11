#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "OpenSLESSoundSystemExtensionInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Unknowable.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenSLESSoundSource;
    //////////////////////////////////////////////////////////////////////////
    class OpenSLESSoundSystem
        : public ServiceBase<SoundSystemInterface>
        , public OpenSLESSoundSystemExtensionInterface
    {
        DECLARE_FACTORABLE( OpenSLESSoundSystem );
        DECLARE_UNKNOWABLE();

    public:
        OpenSLESSoundSystem();
        ~OpenSLESSoundSystem() override;

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
        void onDestroyOpenSLESSoundSource_( OpenSLESSoundSource * _soundSource );

    protected:
        SLObjectItf m_engineObject;
        SLEngineItf m_engineItf;
        SLObjectItf m_outputMixObject;

        FactoryInterfacePtr m_factoryOpenSLESSoundBuffer;
        FactoryInterfacePtr m_factoryOpenSLESSoundBufferStream;
        FactoryInterfacePtr m_factoryOpenSLESSoundSource;
    };
}

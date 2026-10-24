#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "AAudioSoundMixer.h"

#include "Kernel/MixerSoundExtensionInterface.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/BaseApplicationLifecycle.h"
#include "Kernel/Unknowable.h"

#include "Config/Timestamp.h"

#include <aaudio/AAudio.h>

namespace Mengine
{
    class MixerSoundSource;

    class AAudioSoundSystem
        : public ServiceBase<SoundSystemInterface>
        , public MixerSoundExtensionInterface
        , public BaseApplicationLifecycle
    {
        DECLARE_FACTORABLE( AAudioSoundSystem );
        DECLARE_UNKNOWABLE();
        DECLARE_APPLICATION_LIFECYCLEABLE();

    public:
        AAudioSoundSystem();
        ~AAudioSoundSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void update() override;
        bool isSilent() const override;

    public:
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc ) override;
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _streamable, const DocumentInterfacePtr & _doc ) override;

    public:
        bool acquireSourceBus( const SoundSourceInterfacePtr & _source, uint32_t _frequency, uint32_t _channels, float _gain, uint32_t * const _busIndex ) override;
        void releaseSourceBus( uint32_t _busIndex ) override;
        bool setSourceBusVolume( uint32_t _busIndex, float _gain ) override;
        bool enableSourceBus( uint32_t _busIndex, bool _enable ) override;

    protected:
        void _onApplicationDidBecomeActive() override;
        void _onApplicationWillEnterForeground() override;
        void _onApplicationDidEnterBackground() override;
        void _onApplicationWillResignActive() override;
        void _onApplicationWillTerminate() override;

    protected:
        bool openStream_();
        void closeStream_();
        void updateStreamState_();
        void onDestroyMixerSoundSource_( MixerSoundSource * _source );

    protected:
        AAudioSoundMixerPtr m_mixer;
        AAudioStream * m_stream;
        Timestamp m_reopenTimestamp;
        bool m_startRequested;
        bool m_active;

        FactoryInterfacePtr m_factorySoundBuffer;
        FactoryInterfacePtr m_factorySoundBufferStream;
        FactoryInterfacePtr m_factorySoundSource;
    };
}

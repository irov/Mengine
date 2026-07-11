#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "ALSASoundSource.h"
#include "ALSASoundSystemExtensionInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/SpinLock.h"
#include "Kernel/Unknowable.h"

#include "Config/Atomic.h"
#include "Config/StdThread.h"

#include <alsa/asoundlib.h>

#ifndef MENGINE_UNIX_MIXER_INPUT_BUS_COUNT
#define MENGINE_UNIX_MIXER_INPUT_BUS_COUNT (32)
#endif

namespace Mengine
{
    class ALSASoundSystem
        : public ServiceBase<SoundSystemInterface>
        , public ALSASoundSystemExtensionInterface
    {
        DECLARE_FACTORABLE( ALSASoundSystem );
        DECLARE_UNKNOWABLE();

    public:
        ALSASoundSystem();
        ~ALSASoundSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void update() override;
        bool isSilent() const override;

    public:
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, const DocumentInterfacePtr & _doc ) override;
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _isStream, const DocumentInterfacePtr & _doc ) override;

    public:
        bool acquireSourceBus( const SoundSourceInterfacePtr & _source, uint32_t _frequency, uint32_t _channels, float _gain, uint32_t * const _busIndex ) override;
        void releaseSourceBus( uint32_t _busIndex ) override;
        bool setSourceBusVolume( uint32_t _busIndex, float _gain ) override;
        bool enableSourceBus( uint32_t _busIndex, bool _enable ) override;

    protected:
        bool createAudioMixer_();
        void destroyAudioMixer_();
        void renderThread_();
        void mixOutputFrames_( float * _mixBuffer, uint32_t _frames, uint32_t _outputChannels );

        void onDestroyALSASoundSource_( ALSASoundSource * _soundSource );

    protected:
        snd_pcm_t * m_pcm;
        StdThread::thread m_renderThread;
        AtomicBool m_renderRunning;
        AtomicBool m_renderFailed;
        bool m_renderFailureReported;

        uint32_t m_outputSampleRate;
        uint32_t m_outputChannels;
        snd_pcm_uframes_t m_periodFrames;

        SpinLock m_busLock;

        struct ALSASoundMixerBusDesc
        {
            ALSASoundMixerBusDesc()
                : frequency( 0 )
                , channels( 0 )
                , gain( 1.f )
                , enabled( false )
                , busy( false )
            {
            }

            SpinLock lock;
            SoundSourceInterfacePtr source;
            uint32_t frequency;
            uint32_t channels;
            AtomicFloat gain;
            AtomicBool enabled;
            bool busy;
        };

        ALSASoundMixerBusDesc m_mixerBuses[MENGINE_UNIX_MIXER_INPUT_BUS_COUNT];

        FactoryInterfacePtr m_factoryALSASoundBuffer;
        FactoryInterfacePtr m_factoryALSASoundBufferStream;
        FactoryInterfacePtr m_factoryALSASoundSource;
    };
}

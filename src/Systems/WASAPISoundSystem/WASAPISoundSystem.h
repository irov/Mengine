#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "WASAPISoundIncluder.h"
#include "WASAPISoundSource.h"
#include "WASAPISoundSystemExtensionInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/SpinLock.h"
#include "Kernel/Unknowable.h"

#include "Config/Atomic.h"

#ifndef MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT
#define MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT (32)
#endif

namespace Mengine
{
    class WASAPISoundSystem
        : public ServiceBase<SoundSystemInterface>
        , public WASAPISoundSystemExtensionInterface
    {
        DECLARE_FACTORABLE( WASAPISoundSystem );
        DECLARE_UNKNOWABLE();

    public:
        WASAPISoundSystem();
        ~WASAPISoundSystem() override;

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
        bool acquireSourceBus( const SoundSourceInterfacePtr & _source, uint32_t _frequency, uint32_t _channels, float _gain, uint32_t * const _busIndex ) override;
        void releaseSourceBus( uint32_t _busIndex ) override;
        bool setSourceBusVolume( uint32_t _busIndex, float _gain ) override;
        bool enableSourceBus( uint32_t _busIndex, bool _enable ) override;

    protected:
        bool createAudioMixer_();
        void destroyAudioMixer_();

        static DWORD WINAPI renderThreadProc_( LPVOID _param );
        void renderThread_();

        void mixOutputFrames_( float * _mixBuffer, uint32_t _frames, uint32_t _outputChannels );

    protected:
        void onDestroyWASAPISoundSource_( WASAPISoundSource * _soundSource );

    protected:
        HANDLE m_renderThread;
        HANDLE m_stopEvent;
        HANDLE m_initializeEvent;
        AtomicBool m_initializeSuccess;

        uint32_t m_outputSampleRate;
        uint32_t m_outputChannels;

        SpinLock m_busLock;

        struct WASAPISoundMixerBusDesc
        {
            WASAPISoundMixerBusDesc()
                : index( 0 )
                , frequency( 0 )
                , channels( 0 )
                , gain( 1.f )
                , enabled( false )
                , busy( false )
            {
            }

            SpinLock lock;
            SoundSourceInterfacePtr source;
            uint32_t index;
            uint32_t frequency;
            uint32_t channels;
            AtomicFloat gain;
            AtomicBool enabled;
            bool busy;
        };

        WASAPISoundMixerBusDesc m_mixerBuses[MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT];

        FactoryInterfacePtr m_factoryWASAPISoundBuffer;
        FactoryInterfacePtr m_factoryWASAPISoundBufferStream;
        FactoryInterfacePtr m_factoryWASAPISoundSource;
    };
}

#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "AudioUnitSoundSource.h"
#include "AudioUnitSoundSystemExtensionInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/SpinLock.h"
#include "Kernel/Unknowable.h"

#include <AudioToolbox/AudioToolbox.h>

#ifndef MENGINE_APPLE_MIXER_INPUT_BUS_COUNT
#define MENGINE_APPLE_MIXER_INPUT_BUS_COUNT (32)
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AudioUnitSoundSystem
        : public ServiceBase<SoundSystemInterface>
        , public AudioUnitSoundSystemExtensionInterface
    {
        DECLARE_FACTORABLE( AudioUnitSoundSystem );
        DECLARE_UNKNOWABLE();

    public:
        AudioUnitSoundSystem();
        ~AudioUnitSoundSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void update() override;

    public:
        bool isSilent() const override;

    public:
        SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc ) override;
        SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _isStream, const DocumentInterfacePtr & _doc ) override;

    public:
        bool acquireSourceBus( const SoundSourceInterfacePtr & _source, uint32_t _frequency, uint32_t _channels, float _gain, uint32_t * const _busIndex ) override;
        void releaseSourceBus( uint32_t _busIndex ) override;
        bool setSourceBusVolume( uint32_t _busIndex, float _gain ) override;
        bool enableSourceBus( uint32_t _busIndex, bool _enable ) override;

#if defined(MENGINE_PLATFORM_IOS)
    public:
        void handleAudioInterruption_( bool _began );
#endif

    protected:
        bool createAudioMixer_();
        void destroyAudioMixer_();

        bool configureSourceBus_( uint32_t _busIndex, uint32_t _frequency, uint32_t _channels );
        bool enableSourceBus_( uint32_t _busIndex, bool _enable );

    protected:
        static OSStatus mixerRenderCallback_( void * _refCon, AudioUnitRenderActionFlags * _flags, const AudioTimeStamp * _timeStamp, UInt32 _busNumber, UInt32 _numberFrames, AudioBufferList * _ioData );

    protected:
        void onDestroyAudioUnitSoundSource_( AudioUnitSoundSource * _soundSource );

    protected:
        AudioUnit m_outputUnit;
        AudioUnit m_mixerUnit;
        Float64 m_outputSampleRate;
        UInt32 m_outputChannels;

        SpinLock m_busLock;

        struct AudioUnitSoundMixerBusDesc
        {
            SpinLock lock;
            SoundSourceInterfacePtr source;
            uint32_t index = 0;
            uint32_t frequency = 0;
            uint32_t channels = 0;
            bool busy = false;
        };
        AudioUnitSoundMixerBusDesc m_mixerBuses[MENGINE_APPLE_MIXER_INPUT_BUS_COUNT];

        FactoryInterfacePtr m_factoryAudioUnitSoundBuffer;
        FactoryInterfacePtr m_factoryAudioUnitSoundBufferStream;
        FactoryInterfacePtr m_factoryAudioUnitSoundSource;

#if defined(MENGINE_PLATFORM_IOS)
        id m_interruptionObserver;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
}

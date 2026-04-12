#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "AppleSoundSystemExtensionInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/SpinLock.h"
#include "Kernel/Unknowable.h"

#include <AudioToolbox/AudioToolbox.h>
#include <TargetConditionals.h>

#ifndef MENGINE_APPLE_MIXER_INPUT_BUS_COUNT
#define MENGINE_APPLE_MIXER_INPUT_BUS_COUNT (32)
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleSoundSource;
    //////////////////////////////////////////////////////////////////////////
    struct AppleSoundMixerBusDesc
    {
        AppleSoundMixerBusDesc()
            : index( 0 )
            , frequency( 0 )
            , channels( 0 )
            , busy( false )
        {
        }

        SpinLock lock;
        SoundSourceInterfacePtr source;
        uint32_t index;
        uint32_t frequency;
        uint32_t channels;
        bool busy;
    };
    //////////////////////////////////////////////////////////////////////////
    class AppleSoundSystem
        : public ServiceBase<SoundSystemInterface>
        , public AppleSoundSystemExtensionInterface
    {
        DECLARE_FACTORABLE( AppleSoundSystem );
        DECLARE_UNKNOWABLE();

    public:
        AppleSoundSystem();
        ~AppleSoundSystem() override;

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

        bool configureSourceBus_( uint32_t _busIndex, uint32_t _frequency, uint32_t _channels );
        bool enableSourceBus_( uint32_t _busIndex, bool _enable );

        static void makeFormat_( AudioStreamBasicDescription * const _format, Float64 _sampleRate, UInt32 _channels );
        static void zeroBufferList_( AudioBufferList * _ioData );

        static OSStatus mixerRenderCallback_( void * _refCon, AudioUnitRenderActionFlags * _flags, const AudioTimeStamp * _timeStamp, UInt32 _busNumber, UInt32 _numberFrames, AudioBufferList * _ioData );

    protected:
        void onDestroyAppleSoundSource_( AppleSoundSource * _soundSource );

#if TARGET_OS_IPHONE
    public:
        void handleAudioInterruption_( bool _began );

    protected:
        id m_interruptionObserver;
        id m_routeChangeObserver;
        id m_mediaResetObserver;
#endif

    protected:
        AudioUnit m_outputUnit;
        AudioUnit m_mixerUnit;
        Float64 m_outputSampleRate;
        UInt32 m_outputChannels;

        SpinLock m_busLock;
        AppleSoundMixerBusDesc m_mixerBuses[MENGINE_APPLE_MIXER_INPUT_BUS_COUNT];

        FactoryInterfacePtr m_factoryAppleSoundBuffer;
        FactoryInterfacePtr m_factoryAppleSoundBufferStream;
        FactoryInterfacePtr m_factoryAppleSoundSource;
    };
}

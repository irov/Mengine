#include "AudioUnitSoundSystem.h"

#include "Interface/ThreadServiceInterface.h"

#include "AudioUnitSoundSource.h"
#include "AudioUnitSoundBufferMemory.h"
#include "AudioUnitSoundBufferStream.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/SpinLockScope.h"

#include "Config/StdAlgorithm.h"

#if defined(MENGINE_PLATFORM_IOS)
#import "Environment/iOS/iOSLog.h"

#import <AVFoundation/AVFoundation.h>
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::AudioUnitSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void makeFormat( AudioStreamBasicDescription * const _format, Float64 _sampleRate, UInt32 _channels )
        {
            uint8_t * formatBuffer = reinterpret_cast<uint8_t *>(_format);
            StdAlgorithm::fill_n( formatBuffer, sizeof( AudioStreamBasicDescription ), 0 );

            _format->mSampleRate = _sampleRate;
            _format->mFormatID = kAudioFormatLinearPCM;
            AudioFormatFlags formatFlags = static_cast<AudioFormatFlags>(kAudioFormatFlagsNativeFloatPacked);
            formatFlags |= static_cast<AudioFormatFlags>(kAudioFormatFlagIsNonInterleaved);

            _format->mFormatFlags = formatFlags;
            _format->mBytesPerPacket = sizeof( Float32 );
            _format->mFramesPerPacket = 1;
            _format->mBytesPerFrame = sizeof( Float32 );
            _format->mChannelsPerFrame = _channels;
            _format->mBitsPerChannel = sizeof( Float32 ) * 8;
        }
        //////////////////////////////////////////////////////////////////////////
        static void zeroBufferList( AudioBufferList * _ioData )
        {
            if( _ioData == nullptr )
            {
                return;
            }

            for( UInt32 index = 0; index != _ioData->mNumberBuffers; ++index )
            {
                AudioBuffer & buffer = _ioData->mBuffers[index];
                uint8_t * bufferData = reinterpret_cast<uint8_t *>(buffer.mData);
                StdAlgorithm::fill_n( bufferData, buffer.mDataByteSize, 0 );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AudioUnitSoundSystem::AudioUnitSoundSystem()
        : m_outputUnit( nullptr )
        , m_mixerUnit( nullptr )
        , m_outputSampleRate( 44100.0 )
        , m_outputChannels( 2 )
#if defined(MENGINE_PLATFORM_IOS)
        , m_interruptionObserver( nil )
#endif
    {
        for( uint32_t index = 0; index != MENGINE_APPLE_MIXER_INPUT_BUS_COUNT; ++index )
        {
            m_mixerBuses[index].index = index;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AudioUnitSoundSystem::~AudioUnitSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSystem::_initializeService()
    {
#if defined(MENGINE_PLATFORM_IOS)
        @autoreleasepool
        {
            AVAudioSession * session = [AVAudioSession sharedInstance];

            NSError * error = nil;
            if( [session setCategory:AVAudioSessionCategoryAmbient error:&error] == NO )
            {
                IOS_LOGGER_WARNING( @"failed to set AVAudioSession category [%@]"
                    , error.localizedDescription
                );
            }

            if( [session setActive:YES error:&error] == NO )
            {
                IOS_LOGGER_WARNING( @"failed to activate AVAudioSession [%@]"
                    , error.localizedDescription
                );
            }

            double sampleRate = session.sampleRate;

            if( sampleRate > 0.0 )
            {
                m_outputSampleRate = sampleRate;
            }

            AudioUnitSoundSystem * soundSystem = this;

            m_interruptionObserver = [[NSNotificationCenter defaultCenter]
                addObserverForName:AVAudioSessionInterruptionNotification
                object:session
                queue:nil
                usingBlock:^( NSNotification * _notification )
                {
                    NSDictionary * info = _notification.userInfo;
                    NSNumber * interruptionType = info[AVAudioSessionInterruptionTypeKey];
                    AVAudioSessionInterruptionType type = (AVAudioSessionInterruptionType)interruptionType.unsignedIntegerValue;

                    if( type == AVAudioSessionInterruptionTypeBegan )
                    {
                        soundSystem->handleAudioInterruption_( true );
                    }
                    else if( type == AVAudioSessionInterruptionTypeEnded )
                    {
                        soundSystem->handleAudioInterruption_( false );
                    }
                }
            ];
        }
#endif

        if( this->createAudioMixer_() == false )
        {
            LOGGER_ASSERTION( "failed to create apple audio mixer" );

            return false;
        }

        LOGGER_MESSAGE( "AudioUnitSoundSystem initialized with shared AudioUnit mixer backend" );

        m_factoryAudioUnitSoundBuffer = Helper::makeFactoryPoolWithMutex<AudioUnitSoundBufferMemory, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAudioUnitSoundBufferStream = Helper::makeFactoryPoolWithMutex<AudioUnitSoundBufferStream, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAudioUnitSoundSource = Helper::makeFactoryPoolWithMutexAndListener<AudioUnitSoundSource, 32>( this, &AudioUnitSoundSystem::onDestroyAudioUnitSoundSource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAudioUnitSoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAudioUnitSoundBufferStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAudioUnitSoundSource );

#if defined(MENGINE_PLATFORM_IOS)
        if( m_interruptionObserver != nil )
        {
            [[NSNotificationCenter defaultCenter] removeObserver:m_interruptionObserver];
            m_interruptionObserver = nil;
        }
#endif

        this->destroyAudioMixer_();

#if defined(MENGINE_PLATFORM_IOS)
        @autoreleasepool
        {
            AVAudioSession * session = [AVAudioSession sharedInstance];
            NSError * error = nil;
            if( [session setActive:NO withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation error:&error] == NO )
            {
                IOS_LOGGER_WARNING( @"failed to deactivate AVAudioSession [%@]"
                    , error.localizedDescription
                );
            }
        }
#endif

        m_factoryAudioUnitSoundBuffer = nullptr;
        m_factoryAudioUnitSoundBufferStream = nullptr;
        m_factoryAudioUnitSoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSystem::update()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr AudioUnitSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc )
    {
        AudioUnitSoundSourcePtr soundSource = m_factoryAudioUnitSoundSource->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( soundSource, "invalid sound source create" );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        if( soundSource->initialize() == false )
        {
            LOGGER_ASSERTION( "invalid sound source initialize" );

            return nullptr;
        }

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr AudioUnitSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _isStream, const DocumentInterfacePtr & _doc )
    {
        AudioUnitSoundBufferBasePtr base = nullptr;

        bool stream = _isStream;

        if( stream == true )
        {
            stream = SERVICE_IS_INITIALIZE( ThreadServiceInterface );
        }

        if( stream == false )
        {
            AudioUnitSoundBufferMemoryPtr buffer = m_factoryAudioUnitSoundBuffer->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create sound buffer" );

            base = buffer;
        }
        else
        {
            AudioUnitSoundBufferStreamPtr buffer = m_factoryAudioUnitSoundBufferStream->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create sound buffer" );

            base = buffer;
        }

        if( base->load( _decoder ) == false )
        {
            LOGGER_ASSERTION( "failed to create sound buffer from stream" );

            return nullptr;
        }

        return base;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSystem::acquireSourceBus( const SoundSourceInterfacePtr & _source, uint32_t _frequency, uint32_t _channels, float _gain, uint32_t * const _busIndex )
    {
        uint32_t busIndex = MENGINE_APPLE_MIXER_INPUT_BUS_COUNT;

        {
            MENGINE_SPINLOCK_SCOPE( m_busLock );

            for( uint32_t index = 0; index != MENGINE_APPLE_MIXER_INPUT_BUS_COUNT; ++index )
            {
                if( m_mixerBuses[index].busy == true )
                {
                    continue;
                }

                m_mixerBuses[index].busy = true;
                busIndex = index;

                break;
            }
        }

        if( busIndex == MENGINE_APPLE_MIXER_INPUT_BUS_COUNT )
        {
            LOGGER_ASSERTION( "no free apple mixer bus" );

            return false;
        }

        AudioUnitSoundMixerBusDesc & busDesc = m_mixerBuses[busIndex];
        bool configure = busDesc.frequency != _frequency;

        if( configure == false )
        {
            configure = busDesc.channels != _channels;
        }

        if( configure == true )
        {
            if( this->configureSourceBus_( busIndex, _frequency, _channels ) == false )
            {
                MENGINE_SPINLOCK_SCOPE( m_busLock );
                m_mixerBuses[busIndex].busy = false;

                return false;
            }
        }

        {
            MENGINE_SPINLOCK_SCOPE( busDesc.lock );

            busDesc.source = _source;
            busDesc.frequency = _frequency;
            busDesc.channels = _channels;
        }

        if( this->setSourceBusVolume( busIndex, _gain ) == false )
        {
            this->releaseSourceBus( busIndex );

            return false;
        }

        if( this->enableSourceBus_( busIndex, true ) == false )
        {
            this->releaseSourceBus( busIndex );

            return false;
        }

        *_busIndex = busIndex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSystem::releaseSourceBus( uint32_t _busIndex )
    {
        if( _busIndex >= MENGINE_APPLE_MIXER_INPUT_BUS_COUNT )
        {
            return;
        }

        {
            AudioUnitSoundMixerBusDesc & busDesc = m_mixerBuses[_busIndex];
            MENGINE_SPINLOCK_SCOPE( busDesc.lock );

            busDesc.source = nullptr;
            busDesc.frequency = 0;
            busDesc.channels = 0;
        }

        this->setSourceBusVolume( _busIndex, 0.f );
        this->enableSourceBus_( _busIndex, false );
        AudioUnitReset( m_mixerUnit, kAudioUnitScope_Input, _busIndex );

        {
            MENGINE_SPINLOCK_SCOPE( m_busLock );
            m_mixerBuses[_busIndex].busy = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSystem::setSourceBusVolume( uint32_t _busIndex, float _gain )
    {
        if( _busIndex >= MENGINE_APPLE_MIXER_INPUT_BUS_COUNT )
        {
            return false;
        }

        OSStatus status = AudioUnitSetParameter( m_mixerUnit, kMultiChannelMixerParam_Volume, kAudioUnitScope_Input, _busIndex, _gain, 0 );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to set mixer bus volume [%u] [%d]"
                , _busIndex
                , status
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSystem::enableSourceBus( uint32_t _busIndex, bool _enable )
    {
        if( _busIndex >= MENGINE_APPLE_MIXER_INPUT_BUS_COUNT )
        {
            return false;
        }

        if( this->enableSourceBus_( _busIndex, _enable ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSystem::createAudioMixer_()
    {
        AudioComponentDescription mixerDescription = {};

        mixerDescription.componentType = kAudioUnitType_Mixer;
        mixerDescription.componentSubType = kAudioUnitSubType_MultiChannelMixer;
        mixerDescription.componentManufacturer = kAudioUnitManufacturer_Apple;

        AudioComponent mixerComponent = AudioComponentFindNext( nullptr, &mixerDescription );

        if( mixerComponent == nullptr )
        {
            LOGGER_ASSERTION( "invalid mixer audio component" );

            return false;
        }

        OSStatus status = AudioComponentInstanceNew( mixerComponent, &m_mixerUnit );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to create mixer unit [%d]"
                , status
            );

            m_mixerUnit = nullptr;

            return false;
        }

        AudioComponentDescription outputDescription = {};

        outputDescription.componentType = kAudioUnitType_Output;
        outputDescription.componentManufacturer = kAudioUnitManufacturer_Apple;

#if defined(MENGINE_PLATFORM_IOS)
        outputDescription.componentSubType = kAudioUnitSubType_RemoteIO;
#else
        outputDescription.componentSubType = kAudioUnitSubType_DefaultOutput;
#endif

        AudioComponent outputComponent = AudioComponentFindNext( nullptr, &outputDescription );

        if( outputComponent == nullptr )
        {
            LOGGER_ASSERTION( "invalid output audio component" );

            this->destroyAudioMixer_();

            return false;
        }

        status = AudioComponentInstanceNew( outputComponent, &m_outputUnit );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to create output unit [%d]"
                , status
            );

            this->destroyAudioMixer_();

            return false;
        }

#if defined(MENGINE_PLATFORM_IOS)
        UInt32 enableOutput = 1;
        AudioUnitSetProperty( m_outputUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &enableOutput, sizeof( enableOutput ) );

        UInt32 enableInput = 0;
        AudioUnitSetProperty( m_outputUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &enableInput, sizeof( enableInput ) );
#endif

        UInt32 maximumFramesPerSlice = 4096;
        AudioUnitSetProperty( m_mixerUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0, &maximumFramesPerSlice, sizeof( maximumFramesPerSlice ) );
        AudioUnitSetProperty( m_outputUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0, &maximumFramesPerSlice, sizeof( maximumFramesPerSlice ) );

        UInt32 busCount = MENGINE_APPLE_MIXER_INPUT_BUS_COUNT;
        status = AudioUnitSetProperty( m_mixerUnit, kAudioUnitProperty_ElementCount, kAudioUnitScope_Input, 0, &busCount, sizeof( busCount ) );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to set mixer bus count [%d]"
                , status
            );

            this->destroyAudioMixer_();

            return false;
        }

        AudioStreamBasicDescription mixFormat;
        Detail::makeFormat( &mixFormat, m_outputSampleRate, m_outputChannels );

        status = AudioUnitSetProperty( m_mixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &mixFormat, sizeof( mixFormat ) );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to set mixer output format [%d]"
                , status
            );

            this->destroyAudioMixer_();

            return false;
        }

        status = AudioUnitSetProperty( m_outputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &mixFormat, sizeof( mixFormat ) );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to set output unit format [%d]"
                , status
            );

            this->destroyAudioMixer_();

            return false;
        }

        AudioUnitConnection connection;
        connection.sourceAudioUnit = m_mixerUnit;
        connection.sourceOutputNumber = 0;
        connection.destInputNumber = 0;

        status = AudioUnitSetProperty( m_outputUnit, kAudioUnitProperty_MakeConnection, kAudioUnitScope_Input, 0, &connection, sizeof( connection ) );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to connect mixer to output [%d]"
                , status
            );

            this->destroyAudioMixer_();

            return false;
        }

        for( uint32_t index = 0; index != MENGINE_APPLE_MIXER_INPUT_BUS_COUNT; ++index )
        {
            AURenderCallbackStruct callback;
            callback.inputProc = &AudioUnitSoundSystem::mixerRenderCallback_;
            callback.inputProcRefCon = &m_mixerBuses[index];

            status = AudioUnitSetProperty( m_mixerUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, index, &callback, sizeof( callback ) );

            if( status != noErr )
            {
                LOGGER_ASSERTION( "failed to set mixer input callback [%u] [%d]"
                    , index
                    , status
                );

                this->destroyAudioMixer_();

                return false;
            }

            AudioStreamBasicDescription inputFormat;
            Detail::makeFormat( &inputFormat, m_outputSampleRate, 1 );

            status = AudioUnitSetProperty( m_mixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, index, &inputFormat, sizeof( inputFormat ) );

            if( status != noErr )
            {
                LOGGER_ASSERTION( "failed to set initial mixer bus format [%u] [%d]"
                    , index
                    , status
                );

                this->destroyAudioMixer_();

                return false;
            }

            AudioUnitSetParameter( m_mixerUnit, kMultiChannelMixerParam_Volume, kAudioUnitScope_Input, index, 1.f, 0 );
            AudioUnitSetParameter( m_mixerUnit, kMultiChannelMixerParam_Enable, kAudioUnitScope_Input, index, 0.f, 0 );
        }

        status = AudioUnitInitialize( m_mixerUnit );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to initialize mixer unit [%d]"
                , status
            );

            this->destroyAudioMixer_();

            return false;
        }

        status = AudioUnitInitialize( m_outputUnit );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to initialize output unit [%d]"
                , status
            );

            this->destroyAudioMixer_();

            return false;
        }

        AudioUnitSetParameter( m_mixerUnit, kMultiChannelMixerParam_Volume, kAudioUnitScope_Output, 0, 0.f, 0 );

        status = AudioOutputUnitStart( m_outputUnit );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to start output unit [%d]"
                , status
            );

            this->destroyAudioMixer_();

            return false;
        }

        AudioUnitSetParameter( m_mixerUnit, kMultiChannelMixerParam_Volume, kAudioUnitScope_Output, 0, 1.f, 0 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSystem::destroyAudioMixer_()
    {
        for( uint32_t index = 0; index != MENGINE_APPLE_MIXER_INPUT_BUS_COUNT; ++index )
        {
            AudioUnitSoundMixerBusDesc & busDesc = m_mixerBuses[index];
            MENGINE_SPINLOCK_SCOPE( busDesc.lock );

            busDesc.source = nullptr;
            busDesc.frequency = 0;
            busDesc.channels = 0;
            busDesc.busy = false;
        }

        if( m_outputUnit != nullptr )
        {
            AudioUnitSetParameter( m_mixerUnit, kMultiChannelMixerParam_Volume, kAudioUnitScope_Output, 0, 0.f, 0 );

            AudioOutputUnitStop( m_outputUnit );
            AudioUnitUninitialize( m_outputUnit );
            AudioComponentInstanceDispose( m_outputUnit );

            m_outputUnit = nullptr;
        }

        if( m_mixerUnit != nullptr )
        {
            AudioUnitUninitialize( m_mixerUnit );
            AudioComponentInstanceDispose( m_mixerUnit );

            m_mixerUnit = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSystem::configureSourceBus_( uint32_t _busIndex, uint32_t _frequency, uint32_t _channels )
    {
        Float64 sampleRate = (Float64)_frequency;
        AudioStreamBasicDescription inputFormat;
        Detail::makeFormat( &inputFormat, sampleRate, _channels );

        OSStatus status = AudioUnitSetProperty( m_mixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, _busIndex, &inputFormat, sizeof( inputFormat ) );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to configure mixer bus format [%u] freq [%u] channels [%u] output [%g:%u] [%d]"
                , _busIndex
                , _frequency
                , _channels
                , m_outputSampleRate
                , m_outputChannels
                , status
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundSystem::enableSourceBus_( uint32_t _busIndex, bool _enable )
    {
        AudioUnitParameterValue enable = _enable == true ? 1.f : 0.f;
        OSStatus status = AudioUnitSetParameter( m_mixerUnit, kMultiChannelMixerParam_Enable, kAudioUnitScope_Input, _busIndex, enable, 0 );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to %s mixer bus [%u] [%d]"
                , _enable == true ? "enable" : "disable"
                , _busIndex
                , status
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OSStatus AudioUnitSoundSystem::mixerRenderCallback_( void * _refCon, AudioUnitRenderActionFlags * _flags, const AudioTimeStamp * _timeStamp, UInt32 _busNumber, UInt32 _numberFrames, AudioBufferList * _ioData )
    {
        MENGINE_UNUSED( _flags );
        MENGINE_UNUSED( _timeStamp );
        MENGINE_UNUSED( _busNumber );

        AudioUnitSoundMixerBusDesc * busDesc = static_cast<AudioUnitSoundMixerBusDesc *>(_refCon);

        Detail::zeroBufferList( _ioData );

        SoundSourceInterfacePtr source;

        {
            MENGINE_SPINLOCK_SCOPE( busDesc->lock );
            source = busDesc->source;
        }

        if( source == nullptr )
        {
            return noErr;
        }

        AudioUnitSoundSource * appleSource = static_cast<AudioUnitSoundSource *>(source.get());

        OSStatus result = appleSource->renderMixerFrames( _numberFrames, _ioData );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundSystem::onDestroyAudioUnitSoundSource_( AudioUnitSoundSource * _soundSource )
    {
        _soundSource->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_IOS)
    void AudioUnitSoundSystem::handleAudioInterruption_( bool _began )
    {
        if( _began == true )
        {
            IOS_LOGGER_MESSAGE( @"audio session interruption began, stopping output" );

            if( m_outputUnit == nullptr )
            {
                return;
            }

            AudioOutputUnitStop( m_outputUnit );

            return;
        }

        IOS_LOGGER_MESSAGE( @"audio session interruption ended, restarting output" );

        @autoreleasepool
        {
            AVAudioSession * session = [AVAudioSession sharedInstance];
            NSError * error = nil;
            if( [session setActive:YES error:&error] == NO )
            {
                IOS_LOGGER_WARNING( @"failed to reactivate AVAudioSession [%@]"
                    , error.localizedDescription
                );
            }
        }

        if( m_outputUnit == nullptr )
        {
            return;
        }

        OSStatus status = AudioOutputUnitStart( m_outputUnit );

        if( status != noErr )
        {
            IOS_LOGGER_WARNING( @"failed to restart output unit after interruption [%d]"
                , status
            );
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////
}
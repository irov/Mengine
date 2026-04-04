#include "AppleSoundSystem.h"

#include "AppleSoundSource.h"
#include "AppleSoundBufferMemory.h"
#include "AppleSoundBufferStream.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/SpinLockScope.h"

#include "Config/StdAlgorithm.h"

#import <TargetConditionals.h>

#if TARGET_OS_IPHONE
#import <AVFoundation/AVFoundation.h>
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::AppleSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSoundSystem::AppleSoundSystem()
        : m_outputUnit( nullptr )
        , m_mixerUnit( nullptr )
        , m_outputSampleRate( 44100.0 )
        , m_outputChannels( 2 )
    {
        for( uint32_t index = 0; index != MENGINE_APPLE_MIXER_INPUT_BUS_COUNT; ++index )
        {
            m_mixerBuses[index].index = index;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSoundSystem::~AppleSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSystem::_initializeService()
    {
#if TARGET_OS_IPHONE
        @autoreleasepool
        {
            AVAudioSession * session = [AVAudioSession sharedInstance];

            NSError * error = nil;

            if( [session setActive:YES error:&error] == NO )
            {
                LOGGER_WARNING( "failed to activate AVAudioSession [%s]"
                    , error.localizedDescription.UTF8String
                );
            }

            double sampleRate = session.sampleRate;

            if( sampleRate > 0.0 )
            {
                m_outputSampleRate = sampleRate;
            }
        }
#endif

        if( this->createAudioMixer_() == false )
        {
            LOGGER_ASSERTION( "failed to create apple audio mixer" );

            return false;
        }

        LOGGER_MESSAGE( "AppleSoundSystem initialized with shared AudioUnit mixer backend" );

        m_factoryAppleSoundBuffer = Helper::makeFactoryPoolWithMutex<AppleSoundBufferMemory, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAppleSoundBufferStream = Helper::makeFactoryPoolWithMutex<AppleSoundBufferStream, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAppleSoundSource = Helper::makeFactoryPoolWithMutexAndListener<AppleSoundSource, 32>( this, &AppleSoundSystem::onDestroyAppleSoundSource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAppleSoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAppleSoundBufferStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAppleSoundSource );

        this->destroyAudioMixer_();

        m_factoryAppleSoundBuffer = nullptr;
        m_factoryAppleSoundBufferStream = nullptr;
        m_factoryAppleSoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSystem::update()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr AppleSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc )
    {
        AppleSoundSourcePtr soundSource = m_factoryAppleSoundSource->createObject( _doc );

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
    SoundBufferInterfacePtr AppleSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _isStream, const DocumentInterfacePtr & _doc )
    {
        AppleSoundBufferBasePtr base = nullptr;

        if( _isStream == false || SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == false )
        {
            AppleSoundBufferMemoryPtr buffer = m_factoryAppleSoundBuffer->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create sound buffer" );

            base = buffer;
        }
        else
        {
            AppleSoundBufferStreamPtr buffer = m_factoryAppleSoundBufferStream->createObject( _doc );

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
    bool AppleSoundSystem::acquireSourceBus( const SoundSourceInterfacePtr & _source, uint32_t _frequency, uint32_t _channels, float _gain, uint32_t * const _busIndex )
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

        if( m_mixerBuses[busIndex].frequency != _frequency || m_mixerBuses[busIndex].channels != _channels )
        {
            if( this->configureSourceBus_( busIndex, _frequency, _channels ) == false )
            {
                MENGINE_SPINLOCK_SCOPE( m_busLock );
                m_mixerBuses[busIndex].busy = false;

                return false;
            }
        }

        {
            AppleSoundMixerBusDesc & busDesc = m_mixerBuses[busIndex];
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
    void AppleSoundSystem::releaseSourceBus( uint32_t _busIndex )
    {
        if( _busIndex >= MENGINE_APPLE_MIXER_INPUT_BUS_COUNT )
        {
            return;
        }

        this->enableSourceBus_( _busIndex, false );
        this->setSourceBusVolume( _busIndex, 0.f );
        AudioUnitReset( m_mixerUnit, kAudioUnitScope_Input, _busIndex );

        {
            AppleSoundMixerBusDesc & busDesc = m_mixerBuses[_busIndex];
            MENGINE_SPINLOCK_SCOPE( busDesc.lock );

            busDesc.source = nullptr;
            busDesc.frequency = 0;
            busDesc.channels = 0;
        }

        {
            MENGINE_SPINLOCK_SCOPE( m_busLock );
            m_mixerBuses[_busIndex].busy = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSystem::setSourceBusVolume( uint32_t _busIndex, float _gain )
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
                , (int)status
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSystem::enableSourceBus( uint32_t _busIndex, bool _enable )
    {
        if( _busIndex >= MENGINE_APPLE_MIXER_INPUT_BUS_COUNT )
        {
            return false;
        }

        return this->enableSourceBus_( _busIndex, _enable );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSystem::createAudioMixer_()
    {
        AudioComponentDescription mixerDescription;
        StdAlgorithm::fill_n( reinterpret_cast<uint8_t *>( &mixerDescription ), sizeof( mixerDescription ), 0 );

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
                , (int)status
            );

            m_mixerUnit = nullptr;

            return false;
        }

        AudioComponentDescription outputDescription;
        StdAlgorithm::fill_n( reinterpret_cast<uint8_t *>( &outputDescription ), sizeof( outputDescription ), 0 );

        outputDescription.componentType = kAudioUnitType_Output;
        outputDescription.componentManufacturer = kAudioUnitManufacturer_Apple;

#if TARGET_OS_IPHONE
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
                , (int)status
            );

            this->destroyAudioMixer_();

            return false;
        }

#if TARGET_OS_IPHONE
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
                , (int)status
            );

            this->destroyAudioMixer_();

            return false;
        }

        AudioStreamBasicDescription mixFormat;
        AppleSoundSystem::makeFormat_( &mixFormat, m_outputSampleRate, m_outputChannels );

        status = AudioUnitSetProperty( m_mixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &mixFormat, sizeof( mixFormat ) );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to set mixer output format [%d]"
                , (int)status
            );

            this->destroyAudioMixer_();

            return false;
        }

        status = AudioUnitSetProperty( m_outputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &mixFormat, sizeof( mixFormat ) );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to set output unit format [%d]"
                , (int)status
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
                , (int)status
            );

            this->destroyAudioMixer_();

            return false;
        }

        for( uint32_t index = 0; index != MENGINE_APPLE_MIXER_INPUT_BUS_COUNT; ++index )
        {
            AURenderCallbackStruct callback;
            callback.inputProc = &AppleSoundSystem::mixerRenderCallback_;
            callback.inputProcRefCon = &m_mixerBuses[index];

            status = AudioUnitSetProperty( m_mixerUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, index, &callback, sizeof( callback ) );

            if( status != noErr )
            {
                LOGGER_ASSERTION( "failed to set mixer input callback [%u] [%d]"
                    , index
                    , (int)status
                );

                this->destroyAudioMixer_();

                return false;
            }

            AudioStreamBasicDescription inputFormat;
            AppleSoundSystem::makeFormat_( &inputFormat, m_outputSampleRate, 1 );

            status = AudioUnitSetProperty( m_mixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, index, &inputFormat, sizeof( inputFormat ) );

            if( status != noErr )
            {
                LOGGER_ASSERTION( "failed to set initial mixer bus format [%u] [%d]"
                    , index
                    , (int)status
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
                , (int)status
            );

            this->destroyAudioMixer_();

            return false;
        }

        status = AudioUnitInitialize( m_outputUnit );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to initialize output unit [%d]"
                , (int)status
            );

            this->destroyAudioMixer_();

            return false;
        }

        status = AudioOutputUnitStart( m_outputUnit );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to start output unit [%d]"
                , (int)status
            );

            this->destroyAudioMixer_();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSystem::destroyAudioMixer_()
    {
        for( uint32_t index = 0; index != MENGINE_APPLE_MIXER_INPUT_BUS_COUNT; ++index )
        {
            AppleSoundMixerBusDesc & busDesc = m_mixerBuses[index];
            MENGINE_SPINLOCK_SCOPE( busDesc.lock );

            busDesc.source = nullptr;
            busDesc.frequency = 0;
            busDesc.channels = 0;
            busDesc.busy = false;
        }

        if( m_outputUnit != nullptr )
        {
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
    bool AppleSoundSystem::configureSourceBus_( uint32_t _busIndex, uint32_t _frequency, uint32_t _channels )
    {
        AudioStreamBasicDescription inputFormat;
        AppleSoundSystem::makeFormat_( &inputFormat, (Float64)_frequency, _channels );

        OSStatus status = AudioUnitSetProperty( m_mixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, _busIndex, &inputFormat, sizeof( inputFormat ) );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to configure mixer bus format [%u] [%d]"
                , _busIndex
                , (int)status
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundSystem::enableSourceBus_( uint32_t _busIndex, bool _enable )
    {
        OSStatus status = AudioUnitSetParameter( m_mixerUnit, kMultiChannelMixerParam_Enable, kAudioUnitScope_Input, _busIndex, _enable == true ? 1.f : 0.f, 0 );

        if( status != noErr )
        {
            LOGGER_ASSERTION( "failed to %s mixer bus [%u] [%d]"
                , _enable == true ? "enable" : "disable"
                , _busIndex
                , (int)status
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSystem::makeFormat_( AudioStreamBasicDescription * const _format, Float64 _sampleRate, UInt32 _channels )
    {
        StdAlgorithm::fill_n( reinterpret_cast<uint8_t *>( _format ), sizeof( AudioStreamBasicDescription ), 0 );

        _format->mSampleRate = _sampleRate;
        _format->mFormatID = kAudioFormatLinearPCM;
        _format->mFormatFlags = kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved;
        _format->mBytesPerPacket = sizeof( Float32 );
        _format->mFramesPerPacket = 1;
        _format->mBytesPerFrame = sizeof( Float32 );
        _format->mChannelsPerFrame = _channels;
        _format->mBitsPerChannel = sizeof( Float32 ) * 8;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSystem::zeroBufferList_( AudioBufferList * _ioData )
    {
        if( _ioData == nullptr )
        {
            return;
        }

        for( UInt32 index = 0; index != _ioData->mNumberBuffers; ++index )
        {
            AudioBuffer & buffer = _ioData->mBuffers[index];
            StdAlgorithm::fill_n( reinterpret_cast<uint8_t *>(buffer.mData), buffer.mDataByteSize, 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    OSStatus AppleSoundSystem::mixerRenderCallback_( void * _refCon, AudioUnitRenderActionFlags * _flags, const AudioTimeStamp * _timeStamp, UInt32 _busNumber, UInt32 _numberFrames, AudioBufferList * _ioData )
    {
        MENGINE_UNUSED( _flags );
        MENGINE_UNUSED( _timeStamp );
        MENGINE_UNUSED( _busNumber );

        AppleSoundMixerBusDesc * busDesc = static_cast<AppleSoundMixerBusDesc *>( _refCon );

        AppleSoundSystem::zeroBufferList_( _ioData );

        SoundSourceInterfacePtr source;

        {
            MENGINE_SPINLOCK_SCOPE( busDesc->lock );
            source = busDesc->source;
        }

        if( source == nullptr )
        {
            return noErr;
        }

        AppleSoundSource * appleSource = static_cast<AppleSoundSource *>( source.get() );

        return appleSource->renderMixerFrames( _numberFrames, _ioData );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundSystem::onDestroyAppleSoundSource_( AppleSoundSource * _soundSource )
    {
        _soundSource->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
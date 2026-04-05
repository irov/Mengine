#include "WASAPISoundSystem.h"

#include "WASAPISoundSource.h"
#include "WASAPISoundBufferMemory.h"
#include "WASAPISoundBufferStream.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/SpinLockScope.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::WASAPISoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        static const GUID s_wasapiSubTypePCM = {0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
        static const GUID s_wasapiSubTypeFloat = {0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};

        template<class T>
        static void safeRelease_( T * * _ptr )
        {
            if( *_ptr != nullptr )
            {
                (*_ptr)->Release();
                *_ptr = nullptr;
            }
        }

        static float clampMixSample_( float _value )
        {
            if( _value < -1.f )
            {
                return -1.f;
            }

            if( _value > 1.f )
            {
                return 1.f;
            }

            return _value;
        }

        static DWORD resolveChannelMask_( uint32_t _channels )
        {
            switch( _channels )
            {
            case 1:
                return SPEAKER_FRONT_CENTER;
            case 2:
                return SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
            case 4:
                return SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
            case 6:
                return SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
            default:
                return 0;
            }
        }

        static bool isWaveFormatFloat_( const WAVEFORMATEX * _format )
        {
            if( _format == nullptr )
            {
                return false;
            }

            if( _format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT )
            {
                return true;
            }

            if( _format->wFormatTag == WAVE_FORMAT_EXTENSIBLE )
            {
                const WAVEFORMATEXTENSIBLE * extensible = reinterpret_cast<const WAVEFORMATEXTENSIBLE *>(_format);

                return InlineIsEqualGUID( extensible->SubFormat, s_wasapiSubTypeFloat ) != FALSE;
            }

            return false;
        }

        static bool isWaveFormatPCM16_( const WAVEFORMATEX * _format )
        {
            if( _format == nullptr )
            {
                return false;
            }

            if( _format->wBitsPerSample != 16 )
            {
                return false;
            }

            if( _format->wFormatTag == WAVE_FORMAT_PCM )
            {
                return true;
            }

            if( _format->wFormatTag == WAVE_FORMAT_EXTENSIBLE )
            {
                const WAVEFORMATEXTENSIBLE * extensible = reinterpret_cast<const WAVEFORMATEXTENSIBLE *>(_format);

                return InlineIsEqualGUID( extensible->SubFormat, s_wasapiSubTypePCM ) != FALSE;
            }

            return false;
        }

        struct WASAPIRenderDeviceContext
        {
            WASAPIRenderDeviceContext()
                : deviceEnumerator( nullptr )
                , device( nullptr )
                , audioClient( nullptr )
                , renderClient( nullptr )
                , samplesReadyEvent( nullptr )
                , mixBuffer( nullptr )
                , bufferFrameCount( 0 )
                , outputSampleRate( 0 )
                , outputChannels( 0 )
                , outputFloat( false )
                , bitsPerSample( 0 )
                , frameBytes( 0 )
            {
            }

            IMMDeviceEnumerator * deviceEnumerator;
            IMMDevice * device;
            IAudioClient * audioClient;
            IAudioRenderClient * renderClient;
            HANDLE samplesReadyEvent;
            float * mixBuffer;
            UINT32 bufferFrameCount;
            UINT32 outputSampleRate;
            UINT32 outputChannels;
            bool outputFloat;
            WORD bitsPerSample;
            UINT32 frameBytes;
        };

        static void finalizeRenderDevice_( WASAPIRenderDeviceContext * const _context )
        {
            if( _context->audioClient != nullptr )
            {
                _context->audioClient->Stop();
            }

            delete[] _context->mixBuffer;
            _context->mixBuffer = nullptr;

            if( _context->samplesReadyEvent != nullptr )
            {
                ::CloseHandle( _context->samplesReadyEvent );
                _context->samplesReadyEvent = nullptr;
            }

            safeRelease_( &_context->renderClient );
            safeRelease_( &_context->audioClient );
            safeRelease_( &_context->device );
            safeRelease_( &_context->deviceEnumerator );

            _context->bufferFrameCount = 0;
            _context->outputSampleRate = 0;
            _context->outputChannels = 0;
            _context->outputFloat = false;
            _context->bitsPerSample = 0;
            _context->frameBytes = 0;
        }

        static void writeDeviceBuffer_( const WASAPIRenderDeviceContext & _context, BYTE * _buffer, UINT32 _frames )
        {
            if( _buffer == nullptr || _context.mixBuffer == nullptr || _frames == 0 || _context.outputChannels == 0 )
            {
                return;
            }

            UINT32 sampleCount = _frames * _context.outputChannels;

            if( _context.outputFloat == true )
            {
                float * dst = reinterpret_cast<float *>(_buffer);

                for( UINT32 index = 0; index != sampleCount; ++index )
                {
                    dst[index] = clampMixSample_( _context.mixBuffer[index] );
                }

                return;
            }

            int16_t * dst = reinterpret_cast<int16_t *>(_buffer);

            for( UINT32 index = 0; index != sampleCount; ++index )
            {
                float sample = clampMixSample_( _context.mixBuffer[index] );
                dst[index] = (int16_t)(sample * 32767.f);
            }
        }

        static bool initializeRenderDevice_( WASAPIRenderDeviceContext * const _context )
        {
            HRESULT hr = ::CoCreateInstance( __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&_context->deviceEnumerator) );

            if( FAILED( hr ) )
            {
                LOGGER_ASSERTION( "failed to create MMDeviceEnumerator [%u]"
                    , (uint32_t)hr
                );

                return false;
            }

            hr = _context->deviceEnumerator->GetDefaultAudioEndpoint( eRender, eConsole, &_context->device );

            if( FAILED( hr ) )
            {
                LOGGER_ASSERTION( "failed to get default audio endpoint [%u]"
                    , (uint32_t)hr
                );

                return false;
            }

            hr = _context->device->Activate( __uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void **>(&_context->audioClient) );

            if( FAILED( hr ) )
            {
                LOGGER_ASSERTION( "failed to activate IAudioClient [%u]"
                    , (uint32_t)hr
                );

                return false;
            }

            WAVEFORMATEX * mixFormat = nullptr;
            hr = _context->audioClient->GetMixFormat( &mixFormat );

            if( FAILED( hr ) || mixFormat == nullptr )
            {
                LOGGER_ASSERTION( "failed to get mix format [%u]"
                    , (uint32_t)hr
                );

                return false;
            }

            WAVEFORMATEXTENSIBLE desiredFormat;
            StdAlgorithm::fill_n( reinterpret_cast<uint8_t *>( &desiredFormat ), sizeof( desiredFormat ), (uint8_t)0 );

            desiredFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
            desiredFormat.Format.nChannels = mixFormat->nChannels;
            desiredFormat.Format.nSamplesPerSec = mixFormat->nSamplesPerSec;
            desiredFormat.Format.wBitsPerSample = sizeof( float ) * 8;
            desiredFormat.Format.nBlockAlign = desiredFormat.Format.nChannels * sizeof( float );
            desiredFormat.Format.nAvgBytesPerSec = desiredFormat.Format.nBlockAlign * desiredFormat.Format.nSamplesPerSec;
            desiredFormat.Format.cbSize = sizeof( WAVEFORMATEXTENSIBLE ) - sizeof( WAVEFORMATEX );
            desiredFormat.Samples.wValidBitsPerSample = sizeof( float ) * 8;
            desiredFormat.dwChannelMask = mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE
                ? reinterpret_cast<WAVEFORMATEXTENSIBLE *>(mixFormat)->dwChannelMask
                : resolveChannelMask_( mixFormat->nChannels );
            desiredFormat.SubFormat = s_wasapiSubTypeFloat;

            WAVEFORMATEX * closestFormat = nullptr;
            WAVEFORMATEX * selectedFormat = mixFormat;
            bool selectedFloat = isWaveFormatFloat_( mixFormat );

            hr = _context->audioClient->IsFormatSupported( AUDCLNT_SHAREMODE_SHARED, reinterpret_cast<WAVEFORMATEX *>( &desiredFormat ), &closestFormat );

            if( hr == S_OK )
            {
                selectedFormat = reinterpret_cast<WAVEFORMATEX *>( &desiredFormat );
                selectedFloat = true;
            }
            else if( hr == S_FALSE && closestFormat != nullptr && (isWaveFormatFloat_( closestFormat ) == true || isWaveFormatPCM16_( closestFormat ) == true) )
            {
                selectedFormat = closestFormat;
                selectedFloat = isWaveFormatFloat_( closestFormat );
            }

            if( selectedFloat == false && isWaveFormatPCM16_( selectedFormat ) == false )
            {
                LOGGER_ASSERTION( "unsupported shared-mode output format tag [%u] bits [%u] channels [%u]"
                    , selectedFormat->wFormatTag
                    , selectedFormat->wBitsPerSample
                    , selectedFormat->nChannels
                );

                if( closestFormat != nullptr )
                {
                    ::CoTaskMemFree( closestFormat );
                }

                ::CoTaskMemFree( mixFormat );

                return false;
            }

            UINT32 selectedChannels = selectedFormat->nChannels;
            bool outputFloat = selectedFloat;
            WORD bitsPerSample = selectedFormat->wBitsPerSample;
            UINT32 frameBytes = selectedFormat->nBlockAlign;
            UINT32 sampleRate = selectedFormat->nSamplesPerSec;

            REFERENCE_TIME defaultDevicePeriod = 0;
            _context->audioClient->GetDevicePeriod( &defaultDevicePeriod, nullptr );

            REFERENCE_TIME hnsBufferDuration = defaultDevicePeriod * 3;

            const REFERENCE_TIME hnsMinBufferDuration = 200000; // 20ms minimum

            if( hnsBufferDuration < hnsMinBufferDuration )
            {
                hnsBufferDuration = hnsMinBufferDuration;
            }

            hr = _context->audioClient->Initialize( AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST, hnsBufferDuration, 0, selectedFormat, nullptr );

            if( closestFormat != nullptr )
            {
                ::CoTaskMemFree( closestFormat );
            }

            ::CoTaskMemFree( mixFormat );

            if( FAILED( hr ) )
            {
                LOGGER_ASSERTION( "failed to initialize IAudioClient [%u]"
                    , (uint32_t)hr
                );

                return false;
            }

            _context->outputSampleRate = sampleRate;
            _context->outputChannels = selectedChannels;
            _context->outputFloat = outputFloat;
            _context->bitsPerSample = bitsPerSample;
            _context->frameBytes = frameBytes;

            _context->samplesReadyEvent = ::CreateEventW( nullptr, FALSE, FALSE, nullptr );

            if( _context->samplesReadyEvent == nullptr )
            {
                LOGGER_ASSERTION( "failed to create WASAPI ready event" );

                return false;
            }

            hr = _context->audioClient->SetEventHandle( _context->samplesReadyEvent );

            if( FAILED( hr ) )
            {
                LOGGER_ASSERTION( "failed to set IAudioClient event handle [%u]"
                    , (uint32_t)hr
                );

                return false;
            }

            hr = _context->audioClient->GetBufferSize( &_context->bufferFrameCount );

            if( FAILED( hr ) || _context->bufferFrameCount == 0 )
            {
                LOGGER_ASSERTION( "failed to get device buffer size [%u]"
                    , (uint32_t)hr
                );

                return false;
            }

            hr = _context->audioClient->GetService( __uuidof(IAudioRenderClient), reinterpret_cast<void **>(&_context->renderClient) );

            if( FAILED( hr ) )
            {
                LOGGER_ASSERTION( "failed to get IAudioRenderClient [%u]"
                    , (uint32_t)hr
                );

                return false;
            }

            _context->mixBuffer = new float[_context->bufferFrameCount * _context->outputChannels];

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    WASAPISoundSystem::WASAPISoundSystem()
        : m_renderThread( nullptr )
        , m_stopEvent( nullptr )
        , m_initializeEvent( nullptr )
        , m_initializeSuccess( false )
        , m_outputSampleRate( 44100 )
        , m_outputChannels( 2 )
    {
        for( uint32_t index = 0; index != MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT; ++index )
        {
            m_mixerBuses[index].index = index;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    WASAPISoundSystem::~WASAPISoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSystem::_initializeService()
    {
        if( this->createAudioMixer_() == false )
        {
            LOGGER_ASSERTION( "failed to create WASAPI audio mixer" );

            return false;
        }

        m_factoryWASAPISoundBuffer = Helper::makeFactoryPoolWithMutex<WASAPISoundBufferMemory, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryWASAPISoundBufferStream = Helper::makeFactoryPoolWithMutex<WASAPISoundBufferStream, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryWASAPISoundSource = Helper::makeFactoryPoolWithMutexAndListener<WASAPISoundSource, 32>( this, &WASAPISoundSystem::onDestroyWASAPISoundSource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWASAPISoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWASAPISoundBufferStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWASAPISoundSource );

        this->destroyAudioMixer_();

        m_factoryWASAPISoundBuffer = nullptr;
        m_factoryWASAPISoundBufferStream = nullptr;
        m_factoryWASAPISoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSystem::update()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr WASAPISoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc )
    {
        WASAPISoundSourcePtr soundSource = m_factoryWASAPISoundSource->createObject( _doc );

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
    SoundBufferInterfacePtr WASAPISoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _isStream, const DocumentInterfacePtr & _doc )
    {
        WASAPISoundBufferBasePtr base = nullptr;

        if( _isStream == false || SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == false )
        {
            WASAPISoundBufferMemoryPtr buffer = m_factoryWASAPISoundBuffer->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create sound buffer" );

            base = buffer;
        }
        else
        {
            WASAPISoundBufferStreamPtr buffer = m_factoryWASAPISoundBufferStream->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create sound buffer" );

            base = buffer;
        }

        base->setOutputFrequency( m_outputSampleRate );

        if( base->load( _decoder ) == false )
        {
            LOGGER_ASSERTION( "failed to create WASAPI sound buffer from stream" );

            return nullptr;
        }

        return base;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSystem::acquireSourceBus( const SoundSourceInterfacePtr & _source, uint32_t _frequency, uint32_t _channels, float _gain, uint32_t * const _busIndex )
    {
        uint32_t busIndex = MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT;

        {
            MENGINE_SPINLOCK_SCOPE( m_busLock );

            for( uint32_t index = 0; index != MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT; ++index )
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

        if( busIndex == MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT )
        {
            LOGGER_ASSERTION( "no free WASAPI mixer bus" );

            return false;
        }

        {
            WASAPISoundMixerBusDesc & busDesc = m_mixerBuses[busIndex];
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

        if( this->enableSourceBus( busIndex, true ) == false )
        {
            this->releaseSourceBus( busIndex );

            return false;
        }

        *_busIndex = busIndex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSystem::releaseSourceBus( uint32_t _busIndex )
    {
        if( _busIndex >= MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT )
        {
            return;
        }

        this->enableSourceBus( _busIndex, false );
        this->setSourceBusVolume( _busIndex, 0.f );

        {
            WASAPISoundMixerBusDesc & busDesc = m_mixerBuses[_busIndex];
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
    bool WASAPISoundSystem::setSourceBusVolume( uint32_t _busIndex, float _gain )
    {
        if( _busIndex >= MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT )
        {
            return false;
        }

        if( _gain < 0.f )
        {
            _gain = 0.f;
        }

        m_mixerBuses[_busIndex].gain = _gain;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSystem::enableSourceBus( uint32_t _busIndex, bool _enable )
    {
        if( _busIndex >= MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT )
        {
            return false;
        }

        m_mixerBuses[_busIndex].enabled = _enable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundSystem::createAudioMixer_()
    {
        m_initializeSuccess = false;

        m_stopEvent = ::CreateEventW( nullptr, TRUE, FALSE, nullptr );

        if( m_stopEvent == nullptr )
        {
            LOGGER_ASSERTION( "failed to create WASAPI stop event" );

            return false;
        }

        m_initializeEvent = ::CreateEventW( nullptr, TRUE, FALSE, nullptr );

        if( m_initializeEvent == nullptr )
        {
            LOGGER_ASSERTION( "failed to create WASAPI initialize event" );

            this->destroyAudioMixer_();

            return false;
        }

        m_renderThread = ::CreateThread( nullptr, 0, &WASAPISoundSystem::renderThreadProc_, this, 0, nullptr );

        if( m_renderThread == nullptr )
        {
            LOGGER_ASSERTION( "failed to create WASAPI render thread" );

            this->destroyAudioMixer_();

            return false;
        }

        DWORD waitResult = ::WaitForSingleObject( m_initializeEvent, INFINITE );

        if( waitResult != WAIT_OBJECT_0 )
        {
            LOGGER_ASSERTION( "failed to wait for WASAPI initialization event [%u]"
                , (uint32_t)waitResult
            );

            this->destroyAudioMixer_();

            return false;
        }

        ::CloseHandle( m_initializeEvent );
        m_initializeEvent = nullptr;

        if( m_initializeSuccess.load() == false )
        {
            this->destroyAudioMixer_();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSystem::destroyAudioMixer_()
    {
        for( uint32_t index = 0; index != MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT; ++index )
        {
            WASAPISoundMixerBusDesc & busDesc = m_mixerBuses[index];
            MENGINE_SPINLOCK_SCOPE( busDesc.lock );

            busDesc.source = nullptr;
            busDesc.frequency = 0;
            busDesc.channels = 0;
            busDesc.gain = 0.f;
            busDesc.enabled = false;
            busDesc.busy = false;
        }

        if( m_stopEvent != nullptr )
        {
            ::SetEvent( m_stopEvent );
        }

        if( m_renderThread != nullptr )
        {
            ::WaitForSingleObject( m_renderThread, INFINITE );
            ::CloseHandle( m_renderThread );
            m_renderThread = nullptr;
        }

        if( m_initializeEvent != nullptr )
        {
            ::CloseHandle( m_initializeEvent );
            m_initializeEvent = nullptr;
        }

        if( m_stopEvent != nullptr )
        {
            ::CloseHandle( m_stopEvent );
            m_stopEvent = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    DWORD WINAPI WASAPISoundSystem::renderThreadProc_( LPVOID _param )
    {
        WASAPISoundSystem * soundSystem = static_cast<WASAPISoundSystem *>(_param);
        soundSystem->renderThread_();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSystem::renderThread_()
    {
        HRESULT hrInitialize = ::CoInitializeEx( nullptr, COINIT_MULTITHREADED );
        bool needCoUninitialize = SUCCEEDED( hrInitialize );

        if( FAILED( hrInitialize ) && hrInitialize != RPC_E_CHANGED_MODE )
        {
            LOGGER_ASSERTION( "failed to initialize COM for WASAPI thread [%u]"
                , (uint32_t)hrInitialize
            );

            if( m_initializeEvent != nullptr )
            {
                m_initializeSuccess = false;
                ::SetEvent( m_initializeEvent );
            }

            return;
        }

        Detail::WASAPIRenderDeviceContext context;

        if( Detail::initializeRenderDevice_( &context ) == false )
        {
            if( m_initializeEvent != nullptr )
            {
                m_initializeSuccess = false;
                ::SetEvent( m_initializeEvent );
            }

            Detail::finalizeRenderDevice_( &context );

            if( needCoUninitialize == true )
            {
                ::CoUninitialize();
            }

            return;
        }

        m_outputSampleRate = context.outputSampleRate;
        m_outputChannels = context.outputChannels;

        DWORD avrtTaskIndex = 0;
        HANDLE avrtHandle = ::AvSetMmThreadCharacteristicsW( L"Pro Audio", &avrtTaskIndex );

        if( avrtHandle == nullptr )
        {
            LOGGER_WARNING( "failed to set MMCSS thread characteristics [%u]"
                , (uint32_t)::GetLastError()
            );
        }

        BYTE * initialBuffer = nullptr;
        HRESULT hr = context.renderClient->GetBuffer( context.bufferFrameCount, &initialBuffer );

        if( FAILED( hr ) || initialBuffer == nullptr )
        {
            LOGGER_ASSERTION( "failed to lock initial render buffer [%u]"
                , (uint32_t)hr
            );

            if( avrtHandle != nullptr )
            {
                ::AvRevertMmThreadCharacteristics( avrtHandle );
                avrtHandle = nullptr;
            }

            if( m_initializeEvent != nullptr )
            {
                m_initializeSuccess = false;
                ::SetEvent( m_initializeEvent );
            }

            Detail::finalizeRenderDevice_( &context );

            if( needCoUninitialize == true )
            {
                ::CoUninitialize();
            }

            return;
        }

        this->mixOutputFrames_( context.mixBuffer, context.bufferFrameCount, context.outputChannels );
        Detail::writeDeviceBuffer_( context, initialBuffer, context.bufferFrameCount );

        hr = context.renderClient->ReleaseBuffer( context.bufferFrameCount, 0 );

        if( FAILED( hr ) )
        {
            LOGGER_ASSERTION( "failed to release initial render buffer [%u]"
                , (uint32_t)hr
            );

            if( avrtHandle != nullptr )
            {
                ::AvRevertMmThreadCharacteristics( avrtHandle );
                avrtHandle = nullptr;
            }

            if( m_initializeEvent != nullptr )
            {
                m_initializeSuccess = false;
                ::SetEvent( m_initializeEvent );
            }

            Detail::finalizeRenderDevice_( &context );

            if( needCoUninitialize == true )
            {
                ::CoUninitialize();
            }

            return;
        }

        ::ResetEvent( context.samplesReadyEvent );

        hr = context.audioClient->Start();

        if( FAILED( hr ) )
        {
            LOGGER_ASSERTION( "failed to start IAudioClient [%u]"
                , (uint32_t)hr
            );

            if( avrtHandle != nullptr )
            {
                ::AvRevertMmThreadCharacteristics( avrtHandle );
                avrtHandle = nullptr;
            }

            if( m_initializeEvent != nullptr )
            {
                m_initializeSuccess = false;
                ::SetEvent( m_initializeEvent );
            }

            Detail::finalizeRenderDevice_( &context );

            if( needCoUninitialize == true )
            {
                ::CoUninitialize();
            }

            return;
        }

        LOGGER_MESSAGE( "WASAPISoundSystem initialized sample rate: %u channels: %u float: %u"
            , m_outputSampleRate
            , m_outputChannels
            , context.outputFloat == true ? 1U : 0U
        );

        if( m_initializeEvent != nullptr )
        {
            m_initializeSuccess = true;
            ::SetEvent( m_initializeEvent );
        }

        HANDLE waitHandles[2] = {m_stopEvent, context.samplesReadyEvent};
        bool running = true;

        while( running == true )
        {
            DWORD waitResult = ::WaitForMultipleObjects( 2, waitHandles, FALSE, 2000 );

            switch( waitResult )
            {
            case WAIT_OBJECT_0:
                {
                    running = false;
                }break;
            case WAIT_OBJECT_0 + 1:
                {
                    UINT32 padding = 0;
                    HRESULT hrRender = context.audioClient->GetCurrentPadding( &padding );

                    if( FAILED( hrRender ) )
                    {
                        LOGGER_WARNING( "failed to get current padding [%u]"
                            , (uint32_t)hrRender
                        );

                        running = false;

                        break;
                    }

                    UINT32 framesToWrite = context.bufferFrameCount - padding;

                    if( framesToWrite == 0 )
                    {
                        break;
                    }

                    BYTE * data = nullptr;
                    hrRender = context.renderClient->GetBuffer( framesToWrite, &data );

                    if( FAILED( hrRender ) || data == nullptr )
                    {
                        LOGGER_WARNING( "failed to get WASAPI render buffer [%u]"
                            , (uint32_t)hrRender
                        );

                        running = false;

                        break;
                    }

                    this->mixOutputFrames_( context.mixBuffer, framesToWrite, context.outputChannels );
                    Detail::writeDeviceBuffer_( context, data, framesToWrite );

                    hrRender = context.renderClient->ReleaseBuffer( framesToWrite, 0 );

                    if( FAILED( hrRender ) )
                    {
                        LOGGER_WARNING( "failed to release WASAPI render buffer [%u]"
                            , (uint32_t)hrRender
                        );

                        running = false;
                    }
                }break;
            case WAIT_TIMEOUT:
                {
                    LOGGER_WARNING( "WASAPI wait timeout, device may be unavailable" );
                }break;
            default:
                {
                    LOGGER_WARNING( "unexpected WASAPI wait result [%u]"
                        , (uint32_t)waitResult
                    );

                    running = false;
                }break;
            }
        }

        if( avrtHandle != nullptr )
        {
            ::AvRevertMmThreadCharacteristics( avrtHandle );
            avrtHandle = nullptr;
        }

        Detail::finalizeRenderDevice_( &context );

        if( needCoUninitialize == true )
        {
            ::CoUninitialize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSystem::mixOutputFrames_( float * _mixBuffer, uint32_t _frames, uint32_t _outputChannels )
    {
        if( _mixBuffer == nullptr || _frames == 0 || _outputChannels == 0 )
        {
            return;
        }

        StdAlgorithm::fill_n( reinterpret_cast<uint8_t *>(_mixBuffer), (size_t)_frames * _outputChannels * sizeof( float ), (uint8_t)0 );

        for( uint32_t index = 0; index != MENGINE_WASAPI_MIXER_INPUT_BUS_COUNT; ++index )
        {
            WASAPISoundMixerBusDesc & busDesc = m_mixerBuses[index];

            if( busDesc.enabled.load() == false )
            {
                continue;
            }

            float gain = busDesc.gain.load();

            if( gain <= 0.f )
            {
                continue;
            }

            SoundSourceInterfacePtr source;

            {
                MENGINE_SPINLOCK_SCOPE( busDesc.lock );
                source = busDesc.source;
            }

            if( source == nullptr )
            {
                continue;
            }

            WASAPISoundSource * wasapiSource = static_cast<WASAPISoundSource *>(source.get());
            wasapiSource->renderMixerFrames( _frames, _outputChannels, _mixBuffer, gain );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundSystem::onDestroyWASAPISoundSource_( WASAPISoundSource * _soundSource )
    {
        _soundSource->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}

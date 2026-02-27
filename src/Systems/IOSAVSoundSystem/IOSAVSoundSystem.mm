#import "IOSAVSoundSystem.h"
#import "IOSAVSoundBuffer.h"
#import "IOSAVSoundSource.h"

#import "Kernel/EnumeratorHelper.h"
#import "Kernel/FactoryPool.h"
#import "Kernel/AssertionFactory.h"
#import "Kernel/DocumentHelper.h"
#import "Kernel/Logger.h"

#import <AVFoundation/AVFoundation.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( IOSAVSoundSystem, Mengine::IOSAVSoundSystem );
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    IOSAVSoundSystem::IOSAVSoundSystem()
        : m_engineStarted( false )
        , m_interrupted( false )
        , m_engine( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    IOSAVSoundSystem::~IOSAVSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundSystem::_initializeService()
    {
        m_factorySoundBuffer = Helper::makeFactoryPoolWithMutex<IOSAVSoundBuffer, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factorySoundSource = Helper::makeFactoryPoolWithMutex<IOSAVSoundSource, 32>( MENGINE_DOCUMENT_FACTORABLE );

        [AVAudioSession sharedInstance];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundSource );

        m_factorySoundBuffer = nullptr;
        m_factorySoundSource = nullptr;

        this->stopEngine();

        if( m_engine != nullptr )
        {
            AVAudioEngine * engine = (__bridge_transfer AVAudioEngine *)m_engine;
            engine = nil;
        }

        m_engine = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSystem::update()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSystem::onTurnSound( bool _turn )
    {
        if( _turn == true )
        {
            this->startEngine();
        }
        else
        {
            this->stopEngine();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr IOSAVSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _streamable, const DocumentInterfacePtr & _doc )
    {
        IOSAVSoundBufferPtr buffer = m_factorySoundBuffer->createObject( _doc );

        if( buffer == nullptr )
        {
            LOGGER_ERROR( "invalid create buffer" );

            return nullptr;
        }

        buffer->initialize( this, _decoder, _streamable );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr IOSAVSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, const DocumentInterfacePtr & _doc )
    {
        IOSAVSoundSourcePtr source = m_factorySoundSource->createObject( _doc );

        if( source == nullptr )
        {
            LOGGER_ERROR( "invalid create source" );

            return nullptr;
        }

        source->initialize( this, _isHeadMode );
        source->setSoundBuffer( _sample );

        return source;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSystem::startEngine()
    {
        if( m_engineStarted == true )
        {
            return;
        }

        if( m_engine == nullptr )
        {
            AVAudioEngine * engine = [[AVAudioEngine alloc] init];

            m_engine = (__bridge_retained void *)engine;
        }

        AVAudioSession * session = [AVAudioSession sharedInstance];

        NSError * sessionError = nil;
        if( [session setCategory:AVAudioSessionCategoryPlayback error:&sessionError] == NO )
        {
            LOGGER_ERROR( "invalid set audio session category: %s", sessionError.localizedDescription.UTF8String );
        }

        sessionError = nil;
        if( [session setActive:YES error:&sessionError] == NO )
        {
            LOGGER_ERROR( "invalid active audio session: %s", sessionError.localizedDescription.UTF8String );
        }

        AVAudioEngine * engine = (__bridge AVAudioEngine *)m_engine;

        if( engine.isRunning == NO )
        {
            NSError * startError = nil;
            if( [engine startAndReturnError:&startError] == NO )
            {
                LOGGER_ERROR( "invalid start audio engine: %s", startError.localizedDescription.UTF8String );
            }
            else
            {
                m_engineStarted = true;
            }
        }
        else
        {
            m_engineStarted = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSystem::stopEngine()
    {
        if( m_engineStarted == false )
        {
            return;
        }

        AVAudioEngine * engine = (__bridge AVAudioEngine *)m_engine;

        [engine stop];

        m_engineStarted = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSystem::setInterrupted( bool _interrupted )
    {
        m_interrupted = _interrupted;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundSystem::isInterrupted() const
    {
        return m_interrupted;
    }
    //////////////////////////////////////////////////////////////////////////
    void * IOSAVSoundSystem::getEngine() const
    {
        return m_engine;
    }
    //////////////////////////////////////////////////////////////////////////
}

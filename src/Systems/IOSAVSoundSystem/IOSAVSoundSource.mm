#import "IOSAVSoundSource.h"
#import "IOSAVSoundSystem.h"

#import "Kernel/Logger.h"
#import "Config/StdMath.h"

#import <AVFoundation/AVFoundation.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    IOSAVSoundSource::IOSAVSoundSource()
        : m_soundSystem( nullptr )
        , m_headMode( true )
        , m_playing( false )
        , m_paused( false )
        , m_loop( false )
        , m_volume( 1.f )
        , m_position( 0.f )
        , m_startPosition( 0.f )
        , m_playerNode( nullptr )
        , m_currentBuffer( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    IOSAVSoundSource::~IOSAVSoundSource()
    {
        this->stop();
        this->releasePlayer_();
        this->releaseCurrentBuffer_();
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSource::initialize( IOSAVSoundSystem * _soundSystem, bool _headMode )
    {
        m_soundSystem = _soundSystem;
        m_headMode = _headMode;
        m_volume = 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSource::ensurePlayer_()
    {
        if( m_playerNode != nullptr )
        {
            return;
        }

        m_soundSystem->startEngine();

        AVAudioEngine * engine = (__bridge AVAudioEngine *)m_soundSystem->getEngine();

        if( engine == nil )
        {
            LOGGER_ERROR( "invalid audio engine" );
            return;
        }

        AVAudioPlayerNode * player = [[AVAudioPlayerNode alloc] init];

        [engine attachNode:player];
        [engine connect:player to:engine.mainMixerNode format:nil];
        [player setVolume:m_volume];

        m_playerNode = (__bridge_retained void *)player;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSource::releasePlayer_()
    {
        if( m_playerNode == nullptr )
        {
            return;
        }

        AVAudioEngine * engine = (__bridge AVAudioEngine *)m_soundSystem->getEngine();
        AVAudioPlayerNode * player = (__bridge_transfer AVAudioPlayerNode *)m_playerNode;

        if( engine != nil )
        {
            [engine detachNode:player];
        }

        m_playerNode = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSource::releaseCurrentBuffer_()
    {
        if( m_currentBuffer == nullptr )
        {
            return;
        }

        CFRelease( (CFTypeRef)m_currentBuffer );
        m_currentBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundSource::play()
    {
        if( m_soundBuffer == nullptr )
        {
            LOGGER_ERROR( "sound source invalid buffer" );

            return false;
        }

        if( m_soundBuffer->acquireSoundBuffer() == false )
        {
            LOGGER_ERROR( "sound source invalid acquire buffer" );

            return false;
        }

        this->ensurePlayer_();

        if( m_playerNode == nullptr )
        {
            return false;
        }

        AVAudioPlayerNode * player = (__bridge AVAudioPlayerNode *)m_playerNode;

        this->releaseCurrentBuffer_();

        void * nativeBufferPtr = m_soundBuffer->createNativePCMBuffer( m_position );

        if( nativeBufferPtr == nullptr )
        {
            LOGGER_ERROR( "sound source invalid native buffer" );

            return false;
        }

        m_currentBuffer = nativeBufferPtr;

        AVAudioPCMBuffer * nativeBuffer = (__bridge AVAudioPCMBuffer *)nativeBufferPtr;

        AVAudioPlayerNodeBufferOptions options = m_loop ? AVAudioPlayerNodeBufferLoops : 0;

        [player scheduleBuffer:nativeBuffer atTime:nil options:options completionHandler:nil];

        [player setVolume:m_volume];

        if( player.isPlaying == false )
        {
            [player play];
        }

        m_startPosition = m_position;
        m_playing = true;
        m_paused = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSource::stop()
    {
        if( m_playerNode != nullptr )
        {
            AVAudioPlayerNode * player = (__bridge AVAudioPlayerNode *)m_playerNode;

            if( player.isPlaying == true || m_paused == true )
            {
                [player stop];
            }
        }

        this->releaseCurrentBuffer_();

        m_playing = false;
        m_paused = false;
        m_position = 0.f;
        m_startPosition = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSource::pause()
    {
        if( m_playing == false )
        {
            return;
        }

        if( m_playerNode == nullptr )
        {
            return;
        }

        AVAudioPlayerNode * player = (__bridge AVAudioPlayerNode *)m_playerNode;

        float currentPosition = this->getPosition();

        [player pause];

        m_position = currentPosition;
        m_startPosition = m_position;
        m_playing = false;
        m_paused = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundSource::resume()
    {
        if( m_paused == false )
        {
            return false;
        }

        if( m_playerNode == nullptr )
        {
            return false;
        }

        AVAudioPlayerNode * player = (__bridge AVAudioPlayerNode *)m_playerNode;

        [player play];

        m_playing = true;
        m_paused = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundSource::isPlay() const
    {
        return m_playing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundSource::isPause() const
    {
        return m_paused;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSource::setVolume( float _volume )
    {
        m_volume = _volume;

        if( m_playerNode == nullptr )
        {
            return;
        }

        AVAudioPlayerNode * player = (__bridge AVAudioPlayerNode *)m_playerNode;
        [player setVolume:_volume];
    }
    //////////////////////////////////////////////////////////////////////////
    float IOSAVSoundSource::getVolume() const
    {
        return m_volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSource::setLoop( bool _loop )
    {
        m_loop = _loop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundSource::getLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    float IOSAVSoundSource::getDuration() const
    {
        if( m_soundBuffer == nullptr )
        {
            return 0.f;
        }

        const SoundCodecDataInfo & dataInfo = m_soundBuffer->getDataInfo();

        return dataInfo.duration;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IOSAVSoundSource::setPosition( float _position )
    {
        if( mt::equal_f_f( m_position, _position ) == true )
        {
            return true;
        }

        m_position = _position;
        m_startPosition = _position;

        if( m_playing == false && m_paused == false )
        {
            return true;
        }

        bool wasPaused = m_paused;

        this->stop();

        m_position = _position;
        m_startPosition = _position;

        if( wasPaused == true )
        {
            if( this->play() == false )
            {
                return false;
            }

            this->pause();

            return true;
        }

        return this->play();
    }
    //////////////////////////////////////////////////////////////////////////
    float IOSAVSoundSource::getPosition() const
    {
        if( m_playing == false )
        {
            return m_position;
        }

        if( m_playerNode == nullptr )
        {
            return m_position;
        }

        AVAudioPlayerNode * player = (__bridge AVAudioPlayerNode *)m_playerNode;

        AVAudioTime * lastTime = player.lastRenderTime;

        if( lastTime == nil )
        {
            return m_position;
        }

        AVAudioTime * playerTime = [player playerTimeForNodeTime:lastTime];

        if( playerTime == nil )
        {
            return m_position;
        }

        if( playerTime.sampleRate <= 0 )
        {
            return m_position;
        }

        double seconds = (double)playerTime.sampleTime / playerTime.sampleRate;

        return m_startPosition + (float)(seconds * 1000.0);
    }
    //////////////////////////////////////////////////////////////////////////
    void IOSAVSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        this->releaseCurrentBuffer_();

        m_soundBuffer = IOSAVSoundBufferPtr::from( _soundBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & IOSAVSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
}

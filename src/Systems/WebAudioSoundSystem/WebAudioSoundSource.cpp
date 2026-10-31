#include "WebAudioSoundSource.h"

#include "WebAudioBridge.h"
#include "WebAudioSoundBuffer.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    WebAudioSoundSource::WebAudioSoundSource()
        : m_sourceId( mengineAudioSourceCreate() )
        , m_volume( 1.f )
        , m_loop( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    WebAudioSoundSource::~WebAudioSoundSource()
    {
        mengineAudioSourceRelease( m_sourceId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundSource::play()
    {
        if( mengineAudioPlay( m_sourceId ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebAudioSoundSource::stop()
    {
        mengineAudioStop( m_sourceId );
    }
    //////////////////////////////////////////////////////////////////////////
    void WebAudioSoundSource::pause()
    {
        mengineAudioPause( m_sourceId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundSource::resume()
    {
        if( mengineAudioResume( m_sourceId ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundSource::isPlay() const
    {
        if( mengineAudioState( m_sourceId ) != EWASS_PLAYING )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundSource::isPause() const
    {
        if( mengineAudioState( m_sourceId ) != EWASS_PAUSED )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundSource::isFinished() const
    {
        if( mengineAudioState( m_sourceId ) != EWASS_FINISHED )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebAudioSoundSource::setVolume( float _volume )
    {
        m_volume = _volume;

        mengineAudioVolume( m_sourceId, _volume );
    }
    //////////////////////////////////////////////////////////////////////////
    float WebAudioSoundSource::getVolume() const
    {
        return m_volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebAudioSoundSource::setLoop( bool _loop )
    {
        m_loop = _loop;

        mengineAudioLoop( m_sourceId, _loop );
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundSource::getLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    float WebAudioSoundSource::getDuration() const
    {
        if( m_soundBuffer == nullptr )
        {
            return 0.f;
        }

        WebAudioSoundBuffer * buffer = static_cast<WebAudioSoundBuffer *>(m_soundBuffer.get());
        float duration = buffer->getDuration();

        return duration;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebAudioSoundSource::setPosition( float _position )
    {
        if( StdMath::isfinite( _position ) == false )
        {
            return false;
        }

        if( _position < 0.f )
        {
            return false;
        }

        float duration = this->getDuration();
        if( _position > duration )
        {
            return false;
        }

        if( mengineAudioSeek( m_sourceId, _position ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float WebAudioSoundSource::getPosition() const
    {
        float position = mengineAudioPosition( m_sourceId );

        return position;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebAudioSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        this->stop();

        m_soundBuffer = _soundBuffer;

        uint32_t bufferId = 0;
        if( m_soundBuffer != nullptr )
        {
            WebAudioSoundBuffer * buffer = static_cast<WebAudioSoundBuffer *>(m_soundBuffer.get());
            bufferId = buffer->getBufferId();
        }

        mengineAudioSourceBuffer( m_sourceId, bufferId );
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & WebAudioSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
}

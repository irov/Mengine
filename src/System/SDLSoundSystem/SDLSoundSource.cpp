#   include "SDLSoundSource.h"

#   include "SDLSoundSystem.h"

#   include "SDLSoundError.h"

#   include "Logger/Logger.h"

#   include "stdex/allocator.h"

#   include "SDL_mixer.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLSoundSource::SDLSoundSource()
        : m_soundSystem(nullptr)
        , m_volume(1.f)
        , m_playing(false)
        , m_pausing(false)
        , m_loop(false)
        , m_position(0.f)
        , m_channel(-1)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLSoundSource::~SDLSoundSource()
    {
        if(m_channel != -1)
        {
            Mix_HaltChannel(m_channel);
            m_channel = -1;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSource::initialize( SDLSoundSystem * _soundSystem )
    {
        m_soundSystem = _soundSystem;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSoundSource::play()
    {
        if( m_playing == true )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::play alredy play"
                );

            return false;
        }

        if( m_soundBuffer == nullptr || m_soundSystem == nullptr )
        {
            return false;
        }

        if( m_pausing == true )
        {
            if( 0 <= m_channel )
            {
                Mix_Resume(m_channel);
            }

            m_pausing = false;

            return true;
        }

        m_channel = m_soundSystem->findFreeChannel();
        if( 0 <= m_channel )
        {
            m_soundBuffer->play(m_channel, m_loop);
            Mix_Volume(m_channel, static_cast<int>(m_volume * 127.5f));

            m_playing = true;
            m_pausing = false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSource::pause()
    {
        if( m_playing == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::pause invalid 'not playing'"
                );

            return;
        }

        if( m_pausing == true )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::pause invalid 'alredy pausing'"
                );

            return;
        }

        if( 0 <= m_channel )
        {
            Mix_Pause(m_channel);
        }

        m_playing = false;
        m_pausing = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSource::stop()
    {
        if( m_playing == false && m_pausing == false )
        {
            return;
        }

        if( 0 <= m_channel )
        {
            Mix_HaltChannel(m_channel);
        }

        m_position = 0.f;

        m_playing = false;
        m_pausing = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSource::complete()
    {
        m_channel = -1;
        m_position = 0.f;

        m_playing = false;
        m_pausing = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSource::setChannel(int _channel)
    {
        m_channel = _channel;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSoundSource::isPlay() const
    {
        return m_playing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSoundSource::isPause() const
    {
        return m_pausing;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSource::setVolume( float _volume )
    {
        m_volume = _volume;

        if( m_playing == false )
        {
            return;
        }

        if( 0 <= m_channel )
        {
            Mix_Volume(m_channel, static_cast<int>(_volume * 127.5f));
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float SDLSoundSource::getVolume() const
    {
        return m_volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSource::setLoop( bool _loop )
    {
        m_loop = _loop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSoundSource::getLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    float SDLSoundSource::getDuration() const
    {
        if( m_soundBuffer == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::getLengthMs invalid sound buffer"
                );

            return 0.f;
        }

        const SoundDecoderInterfacePtr & decoder = m_soundBuffer->getDecoder();

        if( decoder == nullptr )
        {
            return 0.f;
        }

        const SoundCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        float time_sound = dataInfo->length;

        return time_sound;
    }
    //////////////////////////////////////////////////////////////////////////
    float SDLSoundSource::getPosition() const
    {
        if( m_playing == false )
        {
            return 0.f;
        }

        if( m_pausing == true )
        {
            return m_position;
        }

        float posMs = 0.f;

        return posMs;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSoundSource::setPosition( float _posMs )
    {
        if( m_playing == true )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeSoundSource::setPosMs invalid setup pos if playing! [%f]"
                , _posMs
                );

            return false;
        }

        m_position = _posMs;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        m_soundBuffer = _soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr SDLSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
}	// namespace Menge

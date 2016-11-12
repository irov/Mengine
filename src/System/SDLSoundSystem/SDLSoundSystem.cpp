#   include "SDLSoundSystem.h"

#   include "SDLSoundConfig.h"
#   include "SDLSoundError.h"
#   include "SDLSoundSource.h"

#   include "Logger/Logger.h"

#   include "SDL_audio.h"
#   include "SDL_mixer.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Menge::SDLSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLSoundSystem::SDLSoundSystem()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLSoundSystem::~SDLSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSoundSystem::_initialize()
    {
        LOGGER_INFO(m_serviceProvider)("Starting SDL Sound System...");

        Mix_Init(0);
        const SDL_version* mixerVersion = Mix_Linked_Version();
        LOGGER_INFO(m_serviceProvider)("SDL_mixer version: %d.%d.%d",
                                        mixerVersion->major,
                                        mixerVersion->minor,
                                        mixerVersion->patch);

        const int rate = 44100;
        const int stereo = 2;
        const int chunk = 1024;//(rate * stereo * sizeof(short)) >> 2; // quater a second (250ms)
        if(0 > Mix_OpenAudio(rate, MIX_DEFAULT_FORMAT, stereo, 1024))
        {
            LOGGER_ERROR(m_serviceProvider)("SDLSoundSystem.initialize: Failed, error = %s ..."
                                            , Mix_GetError());
            return false;
        }

        const char* driverName = SDL_GetCurrentAudioDriver();
        if(driverName)
        {
            LOGGER_INFO(m_serviceProvider)("Audio driver: %s",
                                           driverName);
        }

        const int channels = Mix_AllocateChannels(MENGINE_SDL_SOUND_MAX_COUNT);
        if(MENGINE_SDL_SOUND_MAX_COUNT != channels)
        {
            LOGGER_ERROR(m_serviceProvider)("SDLSoundSystem.initialize: Mix_AllocateChannels failed, error = %s ..."
                                            , Mix_GetError());
            return false;
        }

        for (int i = 0; i < MENGINE_SDL_SOUND_MAX_COUNT; ++i)
        {
            m_freeChannels[i] = true;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSystem::_finalize()
    {
        Mix_CloseAudio();
        Mix_Quit();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSystem::update()
    {
        for( int i = 0; i < MENGINE_SDL_SOUND_MAX_COUNT; ++i )
        {
            if( !m_freeChannels[i] )
            {
                const int isPaused = Mix_Paused(i);
                const int isPlaying = Mix_Playing(i);

                // this channel stoped playing, mark it as free
                if( !isPaused && !isPlaying )
                {
                    this->freeChannel(i);
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSystem::onTurnSound( bool _turn )
    {
        ((void)_turn);
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr SDLSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream )
    {
        _isStream = false;

        SDLSoundBufferPtr base;

        if( _isStream == false )
        {
            SDLSoundBufferMemory * buffer = m_poolSoundBuffer.createObject();

            buffer->setServiceProvider( m_serviceProvider );

            base = buffer;
        }
        else
        {
            SDLSoundBufferStream * buffer = m_poolSoundBufferStream.createObject();

            buffer->setServiceProvider( m_serviceProvider );

            base = buffer;
        }

        if( base->load( _soundDecoder ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem: Failed to create sound buffer from stream" 
                );

            return nullptr;
        }

        return base;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr SDLSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
    {
        (void)_isHeadMode;

        SDLSoundSource * soundSource = m_poolSoundSource.createObject();

        soundSource->initialize( m_serviceProvider, this );

        soundSource->setSoundBuffer( _buffer );

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    int SDLSoundSystem::findFreeChannel()
    {
        int channel = -1;
        for( int i = 0; i < MENGINE_SDL_SOUND_MAX_COUNT; ++i )
        {
            if( m_freeChannels[i] )
            {
                m_freeChannels[i] = false;
                channel = i;
                break;
            }
        }

        return channel;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundSystem::freeChannel(int channel)
    {
        if( channel >= 0 && channel < MENGINE_SDL_SOUND_MAX_COUNT )
        {
            m_freeChannels[channel] = true;
        }
    }

}   // namespace Menge

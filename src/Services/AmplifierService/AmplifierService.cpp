#include "AmplifierService.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "AmplifierSoundListener.h"

#include "Engine/ResourceMusic.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionResourceType.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AmplifierService, Mengine::AmplifierService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AmplifierService::AmplifierService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AmplifierService::~AmplifierService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AmplifierService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierService::_finalizeService()
    {
        MENGINE_ASSERTION_FATAL( m_soundIdentity == nullptr, "amplifier invalid finalize" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AmplifierService::playMusic( const ConstString & _resourceName, float _position, bool _looped, const AmplifierMusicCallbackInterfacePtr & _callback )
    {
        MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( _resourceName, ResourceMusicPtr, false, "resource '%s' type does not match 'ResourceMusic'"
            , _resourceName.c_str()
        );

        SoundIdentityInterfacePtr keep_soundIdentity = std::move( m_soundIdentity );

        if( keep_soundIdentity != nullptr )
        {
            ESoundSourceState state = keep_soundIdentity->getState();

            switch( state )
            {
            case ESS_INIT:
                break;
            case ESS_PAUSE:
                break;
            case ESS_PLAY:
                {
                    SOUND_SERVICE()
                        ->stopEmitter( keep_soundIdentity );

                    SOUND_SERVICE()
                        ->releaseSoundIdentity( keep_soundIdentity );
                }break;
            case ESS_STOP:
                break;
            case ESS_END:
                break;
            }
        }

        ResourceMusicPtr resourceMusic = RESOURCE_SERVICE()
            ->getResourceReference( ConstString::none(), _resourceName );

        MENGINE_ASSERTION_FATAL( resourceMusic != nullptr, "can't found resource '%s'"
            , _resourceName.c_str()
        );

        const ContentInterfacePtr & content = resourceMusic->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        bool external = resourceMusic->isExternal();
        float volume = resourceMusic->getVolume();

        SoundBufferInterfacePtr buffer;

        if( fileGroup->isPacked() == false || external == false )
        {
            buffer = SOUND_SERVICE()
                ->createSoundBufferFromFile( content, true, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            FileGroupInterfacePtr defaultFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );;

            ContentInterfacePtr externalContent = Helper::makeFileContent( defaultFileGroup, filePath, MENGINE_DOCUMENT_FACTORABLE );

            buffer = SOUND_SERVICE()
                ->createSoundBufferFromFile( externalContent, true, MENGINE_DOCUMENT_FACTORABLE );
        }

        if( buffer == nullptr )
        {
            LOGGER_ASSERTION( "amplifier can't load sample '%s'"
                , filePath.c_str()
            );

            return false;
        }

        SoundIdentityInterfacePtr soundIdentity = SOUND_SERVICE()
            ->createSoundIdentity( false, buffer, ES_SOURCE_CATEGORY_MUSIC, true, MENGINE_DOCUMENT_MESSAGE( "resource '%s'", _resourceName.c_str() ) );

        MENGINE_ASSERTION_MEMORY_PANIC( soundIdentity, "can't create sound source '%s'"
            , filePath.c_str()
        );

        if( _callback != nullptr )
        {
            SoundListenerInterfacePtr soundListener = Helper::makeFactorableUnique<AmplifierSoundListener>( MENGINE_DOCUMENT_FACTORABLE, _callback );

            soundIdentity->setSoundListener( soundListener );
        }

        if( SOUND_SERVICE()
            ->setSourceVolume( soundIdentity, volume, 0.f, MENGINE_MIXER_VALUE_DEFAULT_SPEED, false ) == false )
        {
            LOGGER_ASSERTION( "amplifier can't set sound '%s' volume '%f'"
                , filePath.c_str()
                , volume
            );

            return false;
        }

        if( SOUND_SERVICE()
            ->setPosition( soundIdentity, _position ) == false )
        {
            LOGGER_ASSERTION( "amplifier can't set sound '%s' pos '%f'"
                , filePath.c_str()
                , _position
            );

            return false;
        }

        if( SOUND_SERVICE()
            ->setLoop( soundIdentity, _looped ) == false )
        {
            LOGGER_ASSERTION( "amplifier can't set sound '%s' loop [%u]"
                , filePath.c_str()
                , _looped
            );

            return false;
        }

        if( SOUND_SERVICE()
            ->playEmitter( soundIdentity ) == false )
        {
            LOGGER_ASSERTION( "amplifier sound emitter '%s' invalid play [%u]"
                , filePath.c_str()
                , soundIdentity->getUniqueIdentity()
            );

            return false;
        }

        m_soundIdentity = soundIdentity;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierService::stopMusic()
    {
        if( m_soundIdentity == nullptr )
        {
            return;
        }

        SoundIdentityInterfacePtr keep_soundIdentity = std::move( m_soundIdentity );

        ESoundSourceState state = keep_soundIdentity->getState();

        if( state == ESS_PAUSE && state == ESS_PLAY )
        {
            SOUND_SERVICE()
                ->stopEmitter( keep_soundIdentity );
        }

        SOUND_SERVICE()
            ->releaseSoundIdentity( keep_soundIdentity );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AmplifierService::pauseMusic()
    {
        if( m_soundIdentity == nullptr )
        {
            return false;
        }

        ESoundSourceState state = m_soundIdentity->getState();

        if( state != ESS_PLAY )
        {
            return false;
        }

        SOUND_SERVICE()
            ->pauseEmitter( m_soundIdentity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AmplifierService::resumeMusic()
    {
        if( m_soundIdentity == nullptr )
        {
            return false;
        }

        ESoundSourceState state = m_soundIdentity->getState();

        if( state != ESS_PAUSE )
        {
            return false;
        }

        SOUND_SERVICE()
            ->resumeEmitter( m_soundIdentity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float AmplifierService::getDuration() const
    {
        if( m_soundIdentity == nullptr )
        {
            return 0.f;
        }

        float duration = SOUND_SERVICE()
            ->getDuration( m_soundIdentity );

        return duration;
    }
    //////////////////////////////////////////////////////////////////////////
    float AmplifierService::getPosition() const
    {
        if( m_soundIdentity == nullptr )
        {
            return 0.f;
        }

        float position = SOUND_SERVICE()
            ->getPosition( m_soundIdentity );

        return position;
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierService::setPosition( float _position )
    {
        if( m_soundIdentity == nullptr )
        {
            return;
        }

        SOUND_SERVICE()
            ->setPosition( m_soundIdentity, _position );
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierService::_stopService()
    {
        this->stopMusic();
    }
    //////////////////////////////////////////////////////////////////////////
}

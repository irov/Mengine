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
        : m_play( false )
        , m_pause( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AmplifierService::~AmplifierService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AmplifierService::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_TURN_SOUND, &AmplifierService::notifyTurnSound_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_TURN_SOUND );

        MENGINE_ASSERTION_FATAL( m_soundIdentity == nullptr, "amplifier invalid finalize" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AmplifierService::playMusic( const ConstString & _resourceName, float _pos, bool _looped, const AmplifierMusicCallbackInterfacePtr & _callback )
    {
        MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( _resourceName, ResourceMusicPtr, false, "resource '%s' type does not match 'ResourceMusic'"
            , _resourceName.c_str()
        );

        if( m_play == true )
        {
            this->stopMusic();
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
            ->createSoundIdentity( false, buffer, ES_SOURCE_CATEGORY_MUSIC, true
                , MENGINE_DOCUMENT_MESSAGE( "resource '%s'", _resourceName.c_str() ) );

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
            ->setPosMs( soundIdentity, _pos ) == false )
        {
            LOGGER_ASSERTION( "amplifier can't set sound '%s' pos '%f'"
                , filePath.c_str()
                , _pos
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
                , soundIdentity->getId()
            );

            return false;
        }

        m_soundIdentity = soundIdentity;

        m_play = true;
        m_pause = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierService::stopMusic()
    {
        m_play = false;
        m_pause = false;

        if( m_soundIdentity != nullptr )
        {
            SoundIdentityInterfacePtr keep_soundIdentity = std::move( m_soundIdentity );

            SOUND_SERVICE()
                ->stopEmitter( keep_soundIdentity );

            SOUND_SERVICE()
                ->releaseSoundSource( keep_soundIdentity );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AmplifierService::pauseMusic()
    {
        if( m_soundIdentity == nullptr )
        {
            return false;
        }

        if( m_play == false )
        {
            return true;
        }

        if( m_pause == true )
        {
            return true;
        }

        m_play = true;
        m_pause = true;

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

        if( m_play == false )
        {
            return true;
        }

        if( m_pause == false )
        {
            return true;
        }

        m_play = true;
        m_pause = false;

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

        float pos = SOUND_SERVICE()
            ->getDuration( m_soundIdentity );

        return pos;
    }
    //////////////////////////////////////////////////////////////////////////
    float AmplifierService::getPosMs() const
    {
        if( m_soundIdentity == nullptr )
        {
            return 0.f;
        }

        float pos = SOUND_SERVICE()
            ->getPosMs( m_soundIdentity );

        return pos;
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierService::setPosMs( float _posMs )
    {
        if( m_soundIdentity == nullptr )
        {
            return;
        }

        SOUND_SERVICE()
            ->setPosMs( m_soundIdentity, _posMs );
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierService::_stopService()
    {
        this->stopMusic();
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierService::notifyTurnSound_( bool _turn )
    {
        if( _turn == true )
        {
            this->resumeMusic();
        }
        else
        {
            this->pauseMusic();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

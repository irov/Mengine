#include "Amplifier.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "Engine/ResourceMusic.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionResourceType.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Interface/ScriptServiceInterface.h"

#   include "AmplifierScriptEmbedding.h"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Amplifier, Mengine::Amplifier );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class MyMusicSoundListener
            : public Factorable
            , public SoundListenerInterface
        {
            DECLARE_FACTORABLE( Amplifier );

        public:
            explicit MyMusicSoundListener( const AmplifierMusicCallbackInterfacePtr & _callback )
                : m_callback( _callback )
            {
            }

            ~MyMusicSoundListener() override
            {
            }

        protected:
            void onSoundPause( const SoundIdentityInterfacePtr & _emitter ) override
            {
                MENGINE_UNUSED( _emitter );

                m_callback->onMusicPause();
            }

            void onSoundResume( const SoundIdentityInterfacePtr & _emitter ) override
            {
                MENGINE_UNUSED( _emitter );

                m_callback->onMusicResume();
            }

            void onSoundStop( const SoundIdentityInterfacePtr & _emitter ) override
            {
                MENGINE_UNUSED( _emitter );

                m_callback->onMusicStop();
            }

            void onSoundEnd( const SoundIdentityInterfacePtr & _emitter ) override
            {
                MENGINE_UNUSED( _emitter );

                m_callback->onMusicEnd();
            }

        protected:
            AmplifierMusicCallbackInterfacePtr m_callback;
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Amplifier::Amplifier()
        : m_play( false )
        , m_pause( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Amplifier::~Amplifier()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Amplifier::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_TURN_SOUND, &Amplifier::notifyTurnSound_, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AmplifierScriptEmbedding" ), Helper::makeFactorableUnique<AmplifierScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AmplifierScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Amplifier::_finalizeService()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_TURN_SOUND );

        MENGINE_ASSERTION_FATAL( m_soundIdentity == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Amplifier::playMusic( const ConstString & _resourceName, float _pos, bool _looped, const AmplifierMusicCallbackInterfacePtr & _callback )
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

        MENGINE_ASSERTION_MEMORY_PANIC( resourceMusic, "can't found resource '%s'"
            , _resourceName.c_str()
        );

        const ContentInterfacePtr & content = resourceMusic->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();
        const ConstString & codecType = content->getCodecType();

        bool external = resourceMusic->isExternal();
        float volume = resourceMusic->getVolume();

        SoundBufferInterfacePtr buffer;

        if( fileGroup->isPacked() == false || external == false )
        {
            buffer = SOUND_SERVICE()
                ->createSoundBufferFromFile( fileGroup, filePath, codecType, true, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
                ->getDefaultFileGroup();

            buffer = SOUND_SERVICE()
                ->createSoundBufferFromFile( defaultFileGroup, filePath, codecType, true, MENGINE_DOCUMENT_FACTORABLE );
        }

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "can't load sample '%s'"
            , filePath.c_str()
        );

        SoundIdentityInterfacePtr soundIdentity = SOUND_SERVICE()
            ->createSoundIdentity( false, buffer, ES_SOURCE_CATEGORY_MUSIC, true
                , MENGINE_DOCUMENT_MESSAGE( "resource '%s'", _resourceName.c_str() ) );

        MENGINE_ASSERTION_MEMORY_PANIC( soundIdentity, "can't create sound source '%s'"
            , filePath.c_str()
        );

        if( _callback != nullptr )
        {
            SoundListenerInterfacePtr soundListener = Helper::makeFactorableUnique<Detail::MyMusicSoundListener>( MENGINE_DOCUMENT_FACTORABLE, _callback );

            soundIdentity->setSoundListener( soundListener );
        }

        if( SOUND_SERVICE()
            ->setSourceVolume( soundIdentity, volume, 0.f, false ) == false )
        {
            LOGGER_ERROR( "can't set sound '%s' volume '%f'"
                , filePath.c_str()
                , volume
            );

            return false;
        }

        if( SOUND_SERVICE()
            ->setPosMs( soundIdentity, _pos ) == false )
        {
            LOGGER_ERROR( "can't set sound '%s' pos '%f'"
                , filePath.c_str()
                , _pos
            );

            return false;
        }

        if( SOUND_SERVICE()
            ->setLoop( soundIdentity, _looped ) == false )
        {
            LOGGER_ERROR( "can't set sound '%s' loop [%u]"
                , filePath.c_str()
                , _looped
            );

            return false;
        }

        if( SOUND_SERVICE()
            ->playEmitter( soundIdentity ) == false )
        {
            LOGGER_ERROR( "sound emitter '%s' invalid play [%u]"
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
    void Amplifier::stopMusic()
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
    bool Amplifier::pauseMusic()
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
    bool Amplifier::resumeMusic()
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
    float Amplifier::getDuration() const
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
    float Amplifier::getPosMs() const
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
    void Amplifier::setPosMs( float _posMs )
    {
        if( m_soundIdentity == nullptr )
        {
            return;
        }

        SOUND_SERVICE()
            ->setPosMs( m_soundIdentity, _posMs );
    }
    //////////////////////////////////////////////////////////////////////////
    void Amplifier::_stopService()
    {
        this->stopMusic();
    }
    //////////////////////////////////////////////////////////////////////////
    void Amplifier::notifyTurnSound_( bool _turn )
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

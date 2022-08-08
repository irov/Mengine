#include "SoundService.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/TimepipeServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/Algorithm.h"

#include "math/utils.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundService, Mengine::SoundService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SoundService::SoundService()
        : m_timepipeId( INVALID_UNIQUE_ID )
        , m_supportStream( true )
        , m_muted( false )
        , m_turnStream( true )
        , m_turnSound( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundService::~SoundService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & SoundService::requiredServices() const
    {
        static ServiceRequiredList required = {
            ThreadServiceInterface::getStaticServiceID()
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::_initializeService()
    {
        m_supportStream = THREAD_SERVICE()
            ->isAvailableService();

        if( m_supportStream == true )
        {
            ThreadJobPtr threadJobSoundBufferUpdate = THREAD_SERVICE()
                ->createJob( 25, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( threadJobSoundBufferUpdate );

            m_threadJobSoundBufferUpdate = threadJobSoundBufferUpdate;

            if( THREAD_SERVICE()
                ->createThread( STRINGIZE_STRING_LOCAL( "ThreadSoundBufferUpdate" ), ETP_NORMAL, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }

            if( THREAD_SERVICE()
                ->addTask( STRINGIZE_STRING_LOCAL( "ThreadSoundBufferUpdate" ), m_threadJobSoundBufferUpdate, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }
        }

        float Engine_CommonVolume = CONFIG_VALUE( "Engine", "CommonVolume", 1.f );
        this->setCommonVolume( STRINGIZE_STRING_LOCAL( "Generic" ), Engine_CommonVolume, 0.f );

        float Engine_SoundVolume = CONFIG_VALUE( "Engine", "SoundVolume", 1.f );
        this->setSoundVolume( STRINGIZE_STRING_LOCAL( "Generic" ), Engine_SoundVolume, 0.f );

        float Engine_MusicVolume = CONFIG_VALUE( "Engine", "MusicVolume", 1.f );
        this->setMusicVolume( STRINGIZE_STRING_LOCAL( "Generic" ), Engine_MusicVolume, 0.f );

        float Engine_VoiceVolume = CONFIG_VALUE( "Engine", "VoiceVolume", 1.f );
        this->setVoiceVolume( STRINGIZE_STRING_LOCAL( "Generic" ), Engine_VoiceVolume, 0.f );

        bool OPTION_musicoff = HAS_OPTION( "musicoff" ) || HAS_OPTION( "nomusic" );

        if( OPTION_musicoff == true )
        {
            this->setMusicVolume( STRINGIZE_STRING_LOCAL( "__MusicOFF__" ), 0.f, 0.f );
        }

        m_factoryWorkerTaskSoundBufferUpdate = Helper::makeFactoryPool<ThreadWorkerSoundBufferUpdate, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factorySoundIdentity = Helper::makeFactoryPool<SoundIdentity, 32>( MENGINE_DOCUMENT_FACTORABLE );

        UniqueId timepipe = TIMEPIPE_SERVICE()
            ->addTimepipe( TimepipeInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        m_timepipeId = timepipe;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE, &SoundService::notifyEnginePrepareFinalize, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );

        this->stopSounds_();

        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            this->stopSoundBufferUpdate_( identity );

            identity->finalize();
        }

        m_soundIdentities.clear();

        if( m_threadJobSoundBufferUpdate != nullptr )
        {
            THREAD_SERVICE()
                ->joinTask( m_threadJobSoundBufferUpdate );

            m_threadJobSoundBufferUpdate = nullptr;
        }

        if( m_supportStream == true )
        {
            THREAD_SERVICE()
                ->destroyThread( STRINGIZE_STRING_LOCAL( "ThreadSoundBufferUpdate" ) );
        }

        m_soundVolumeProviders.clear();

        if( m_timepipeId != INVALID_UNIQUE_ID )
        {
            TIMEPIPE_SERVICE()
                ->removeTimepipe( m_timepipeId );

            m_timepipeId = INVALID_UNIQUE_ID;
        }

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWorkerTaskSoundBufferUpdate );

        m_factorySoundIdentity = nullptr;
        m_factoryWorkerTaskSoundBufferUpdate = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::isSupportStreamSound() const
    {
        return m_supportStream;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::addSoundVolumeProvider( const SoundVolumeProviderInterfacePtr & _soundVolumeProvider )
    {
        m_soundVolumeProviders.emplace_back( _soundVolumeProvider );

        this->updateSoundVolumeProvider_( _soundVolumeProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::removeSoundVolumeProvider( const SoundVolumeProviderInterfacePtr & _soundVolumeProvider )
    {
        VectorSoundVolumeProviders::iterator it_found = Algorithm::find( m_soundVolumeProviders.begin(), m_soundVolumeProviders.end(), _soundVolumeProvider );

        if( it_found == m_soundVolumeProviders.end() )
        {
            return false;
        }

        m_soundVolumeProviders.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::playSounds_()
    {
        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            identity->turn = true;

            if( identity->state != ESS_PLAY )
            {
                continue;
            }

            this->updateSourceVolume_( identity );

            const SoundSourceInterfacePtr & source = identity->getSoundSource();

            if( source->play() == false )
            {
                LOGGER_ERROR( "invalid play" );

                continue;
            }

            this->playSoundBufferUpdate_( identity );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::pauseSounds_()
    {
        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            identity->turn = false;

            if( identity->state != ESS_PLAY )
            {
                continue;
            }

            this->pauseSoundBufferUpdate_( identity );

            const SoundSourceInterfacePtr & source = identity->getSoundSource();

            source->pause();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::resumeSounds_()
    {
        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            identity->turn = true;

            if( identity->state != ESS_PLAY )
            {
                continue;
            }

            this->updateSourceVolume_( identity );

            const SoundSourceInterfacePtr & source = identity->getSoundSource();

            if( source->isPlay() == true )
            {
                if( source->resume() == false )
                {
                    LOGGER_ERROR( "invalid resume (play)" );

                    continue;
                }

                this->resumeSoundBufferUpdate_( identity );
            }
            else
            {
                if( source->play() == false )
                {
                    LOGGER_ERROR( "invalid resume (stop)" );

                    continue;
                }

                this->playSoundBufferUpdate_( identity );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::stopSounds_()
    {
        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            identity->turn = false;

            if( identity->state != ESS_PLAY )
            {
                continue;
            }

            this->stopSoundBufferUpdate_( identity );

            const SoundSourceInterfacePtr & source = identity->getSoundSource();

            if( source != nullptr )
            {
                source->stop();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::onTurnStream( bool _turn )
    {
        if( m_turnStream == _turn )
        {
            return;
        }

        m_turnStream = _turn;

        if( m_turnStream == true )
        {
            this->resumeSounds_();
        }
        else
        {
            this->pauseSounds_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::onTurnSound( bool _turn )
    {
        if( m_turnSound == _turn )
        {
            return;
        }

        m_turnSound = _turn;

        SOUND_SYSTEM()
            ->onTurnSound( m_turnSound );
    }
    //////////////////////////////////////////////////////////////////////////
    SoundIdentityInterfacePtr SoundService::createSoundIdentity( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, ESoundSourceCategory _category, bool _streamable, const DocumentPtr & _doc )
    {
        if( m_supportStream == false && _streamable == true )
        {
            LOGGER_WARNING( "unsupport stream sound" );

            _streamable = false;
        }

        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stoped" );

            return nullptr;
        }

        SoundSourceInterfacePtr source = SOUND_SYSTEM()
            ->createSoundSource( _isHeadMode, _buffer, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( source, "create SoundSource invalid" );

        SoundIdentityPtr identity = m_factorySoundIdentity->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity );

        UniqueId new_id = Helper::generateUniqueIdentity();

        identity->setId( new_id );

        identity->setSoundSource( source );
        identity->listener = nullptr;
        identity->worker = nullptr;
        identity->workerId = 0;

        identity->time_left = 0.f;

        identity->state = ESS_STOP;
        identity->category = _category;

        identity->streamable = _streamable;
        identity->looped = false;
        identity->turn = _streamable ? m_turnStream : m_turnSound;

#ifdef MENGINE_DOCUMENT_ENABLE
        identity->doc = _doc;
#endif

        if( identity->initialize() == false )
        {
            LOGGER_ERROR( "invalide initialize identity sound" );

            return nullptr;
        }

        this->updateSourceVolume_( identity );

        m_soundIdentities.emplace_back( identity );

        return identity;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::updateSourceVolume_( const SoundIdentityPtr & _emitter )
    {
        const SoundSourceInterfacePtr & source = _emitter->getSoundSource();

        if( source == nullptr )
        {
            return;
        }

        bool streamable = _emitter->isStreamable();

        bool mute = this->isMute();

        if( mute == true ||
            (m_turnStream == false && streamable == true) ||
            (m_turnSound == false && streamable == false) )
        {
            source->setVolume( 0.f );
        }
        else
        {
            const MixerValue & emitterVolume = _emitter->getVolume();

            ESoundSourceCategory type = _emitter->getCategory();

            switch( type )
            {
            case ES_SOURCE_CATEGORY_SOUND:
                {
                    float commonMixVolume = m_commonVolume.mixValue();
                    float soundMixVolume = m_soundVolume.mixValue();
                    float emitterMixVolume = emitterVolume.mixValue();

                    float mixVolume = 1.f;
                    mixVolume *= commonMixVolume;
                    mixVolume *= soundMixVolume;
                    mixVolume *= emitterMixVolume;

                    source->setVolume( mixVolume );
                }break;
            case ES_SOURCE_CATEGORY_MUSIC:
                {
                    float commonMixVolume = m_commonVolume.mixValue();
                    float musicMixVolume = m_musicVolume.mixValue();
                    float emitterMixVolume = emitterVolume.mixValue();

                    float mixVolume = 1.f;
                    mixVolume *= commonMixVolume;
                    mixVolume *= musicMixVolume;
                    mixVolume *= emitterMixVolume;

                    source->setVolume( mixVolume );
                }break;
            case ES_SOURCE_CATEGORY_VOICE:
                {
                    float commonMixVolume = m_commonVolume.mixValue();
                    float voiceMixVolume = m_voiceVolume.mixValue();
                    float emitterMixVolume = emitterVolume.mixValue();

                    float mixVolume = 1.f;
                    mixVolume *= commonMixVolume;
                    mixVolume *= voiceMixVolume;
                    mixVolume *= emitterMixVolume;

                    source->setVolume( mixVolume );
                }break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    SoundDecoderInterfacePtr SoundService::createSoundDecoder_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, bool _streamable, const DocumentPtr & _doc )
    {
        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, _streamable, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't open sound file '%s'"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
        );

        SoundDecoderInterfacePtr soundDecoder = CODEC_SERVICE()
            ->createDecoder( _codecType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( soundDecoder, "can't create sound decoder for file '%s'"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
        );

        if( soundDecoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "can't initialize sound decoder for file '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            return nullptr;
        }

        return soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr SoundService::createSoundBufferFromFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, bool _streamable, const DocumentPtr & _doc )
    {
        if( m_supportStream == false && _streamable == true )
        {
            LOGGER_WARNING( "unsupport stream sound '%s'"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            );

            _streamable = false;
        }

        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stoped" );

            return nullptr;
        }

        SoundDecoderInterfacePtr soundDecoder;
        if( _streamable == false )
        {
            if( PREFETCHER_SERVICE()
                ->getSoundDecoder( _fileGroup, _filePath, &soundDecoder ) == false )
            {
                soundDecoder = this->createSoundDecoder_( _fileGroup, _filePath, _codecType, false, _doc );
            }
            else
            {
                if( soundDecoder->rewind() == false )
                {
                    LOGGER_ERROR( "invalid rewind decoder '%s'"
                        , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                    );

                    return nullptr;
                }
            }
        }
        else
        {
            soundDecoder = this->createSoundDecoder_( _fileGroup, _filePath, _codecType, true, _doc );
        }

        MENGINE_ASSERTION_MEMORY_PANIC( soundDecoder, "invalid create decoder '%s' type '%s'"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
            , _codecType.c_str()
        );

        if( soundDecoder == nullptr )
        {
            LOGGER_ERROR( "invalid create sound decoder '%s' codec '%s' streamable [%s]"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , _codecType.c_str()
                , _streamable == true ? "true" : "false"
            );

            return nullptr;
        }

        SoundBufferInterfacePtr buffer = SOUND_SYSTEM()
            ->createSoundBuffer( soundDecoder, _streamable, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "can't create sound buffer for file '%s'"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
        );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::releaseSoundSource( const SoundIdentityInterfacePtr & _identity )
    {
        this->stopSoundBufferUpdate_( _identity );

        _identity->finalize();

        uint32_t id = _identity->getId();

        VectorSoundIdentity::iterator it_found = Algorithm::find_if( m_soundIdentities.begin(), m_soundIdentities.end(), [id]( const SoundIdentityPtr & _identity )
        {
            return _identity->getId() == id;
        } );

        if( it_found == m_soundIdentities.end() )
        {
            return false;
        }

        m_soundIdentities.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::setSoundVolume( const ConstString & _type, float _volume, float _default )
    {
        m_soundVolume.setValue( _type, _volume, _default, true );

        this->updateVolume();
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getSoundVolume( const ConstString & _type ) const
    {
        float volume = m_soundVolume.getValue( _type );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::mixSoundVolume() const
    {
        float volume = m_soundVolume.mixValue();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::setCommonVolume( const ConstString & _type, float _volume, float _default )
    {
        m_commonVolume.setValue( _type, _volume, _default, true );

        this->updateVolume();
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getCommonVolume( const ConstString & _type ) const
    {
        float volume = m_commonVolume.getValue( _type );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::mixCommonVolume() const
    {
        float volume = m_commonVolume.mixValue();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::setMusicVolume( const ConstString & _type, float _volume, float _default )
    {
        m_musicVolume.setValue( _type, _volume, _default, true );

        this->updateVolume();
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getMusicVolume( const ConstString & _type ) const
    {
        float volume = m_musicVolume.getValue( _type );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::mixMusicVolume() const
    {
        float volume = m_musicVolume.mixValue();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::setVoiceVolume( const ConstString & _type, float _volume, float _default )
    {
        m_voiceVolume.setValue( _type, _volume, _default, true );

        this->updateVolume();
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getVoiceVolume( const ConstString & _type ) const
    {
        float volume = m_voiceVolume.getValue( _type );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::mixVoiceVolume() const
    {
        float volume = m_voiceVolume.mixValue();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::onTimepipe( const UpdateContext * _context )
    {
        SOUND_SYSTEM()
            ->update();

        bool process = false;

        if( m_commonVolume.update( _context ) == true )
        {
            process = true;
        }

        if( m_soundVolume.update( _context ) == true )
        {
            process = true;
        }

        if( m_musicVolume.update( _context ) == true )
        {
            process = true;
        }

        if( m_voiceVolume.update( _context ) == true )
        {
            process = true;
        }

        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            if( identity->state != ESS_PLAY )
            {
                continue;
            }

            bool source_process = process;

            if( identity->volume.update( _context ) == true )
            {
                source_process = true;
            }

            if( source_process == true )
            {
                this->updateSourceVolume_( identity );
            }

            if( identity->looped == true )
            {
                continue;
            }

            if( identity->turn == false )
            {
                continue;
            }

            const ThreadWorkerSoundBufferUpdatePtr & worker = identity->worker;

            float time_left = identity->time_left;

            if( worker != nullptr )
            {
                if( worker->isDone() == true )
                {
                    identity->state = ESS_STOP;

                    this->stopSoundBufferUpdate_( identity );

                    const SoundSourceInterfacePtr & soundSource = identity->getSoundSource();

                    if( soundSource != nullptr )
                    {
                        soundSource->stop();
                    }

                    identity->time_left = 0.f;

                    if( identity->listener != nullptr )
                    {
                        m_soundIdentitiesAux.emplace_back( identity );
                    }
                }
                else
                {
                    identity->time_left -= _context->time;
                }
            }
            else
            {
                if( time_left - _context->time <= 0.f )
                {
                    identity->state = ESS_STOP;

                    const SoundSourceInterfacePtr & soundSource = identity->getSoundSource();

                    if( soundSource != nullptr )
                    {
                        soundSource->stop();
                    }

                    identity->time_left = 0.f;

                    if( identity->listener != nullptr )
                    {
                        m_soundIdentitiesAux.emplace_back( identity );
                    }
                }
                else
                {
                    identity->time_left -= _context->time;
                }
            }
        }

        if( process == true )
        {
            this->updateVolume();
        }

        for( const SoundIdentityInterfacePtr & identity : m_soundIdentitiesAux )
        {
            SoundListenerInterfacePtr keep_listener = identity->popSoundListener();

            keep_listener->onSoundEnd( identity );
        }

        m_soundIdentitiesAux.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::mute( bool _mute )
    {
        m_muted = _mute;

        this->updateVolume();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::isMute() const
    {
        return m_muted;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::playEmitter( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr" );

            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        ESoundSourceState state = identity->getState();

        switch( state )
        {
        case ESS_STOP:
            {
                if( this->checkMaxSoundPlay_() == false )
                {
                    return false;
                }

                this->updateSourceVolume_( identity );

                const SoundSourceInterfacePtr & source = identity->getSoundSource();

                if( source == nullptr )
                {
                    LOGGER_ERROR( "invalid play %u source is nullptr"
                        , identity->getId()
                    );

                    return false;
                }

                float duration = source->getDuration();
                float position = source->getPosition();

                identity->time_left = duration - position;

                identity->state = ESS_PLAY;

                if( identity->turn == true )
                {
                    if( source->play() == false )
                    {
                        LOGGER_ERROR( "invalid play %u"
                            , identity->getId()
                        );

                        return false;
                    }

                    this->playSoundBufferUpdate_( identity );
                }
            }break;
        case ESS_PAUSE:
            {
                if( this->checkMaxSoundPlay_() == false )
                {
                    return false;
                }

                this->updateSourceVolume_( identity );

                const SoundSourceInterfacePtr & source = identity->getSoundSource();

                float duration = source->getDuration();
                float position = source->getPosition();

                identity->time_left = duration - position;

                identity->state = ESS_PLAY;

                if( identity->turn == true )
                {
                    if( source->resume() == false )
                    {
                        LOGGER_ERROR( "invalid play %u"
                            , identity->getId()
                        );

                        return false;
                    }

                    this->resumeSoundBufferUpdate_( identity );
                }

                const SoundListenerInterfacePtr & listener = identity->getSoundListener();

                if( listener != nullptr )
                {
                    SoundListenerInterfacePtr keep_listener = listener;
                    keep_listener->onSoundResume( identity );
                }
            }break;
        default:
            {
#ifdef MENGINE_DEBUG
                LOGGER_WARNING( "invalid state [%u] (doc: %s)"
                    , identity->state
                    , MENGINE_DOCUMENT_STR( identity->doc )
                );
#endif

                return false;
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::pauseEmitter( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr" );

            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        ESoundSourceState state = identity->getState();

        switch( state )
        {
        case ESS_PLAY:
            {
                identity->state = ESS_PAUSE;

                if( identity->turn == true )
                {
                    this->pauseSoundBufferUpdate_( identity );
                }

                const SoundSourceInterfacePtr & source = identity->getSoundSource();

                source->pause();

                float duration = source->getDuration();
                float position = source->getPosition();

                identity->time_left = duration - position;

                const SoundListenerInterfacePtr & listener = identity->getSoundListener();

                if( listener != nullptr )
                {
                    SoundListenerInterfacePtr keep_listener = listener;
                    keep_listener->onSoundPause( identity );
                }
            }break;
        default:
            {
#ifdef MENGINE_DEBUG
                LOGGER_WARNING( "invalid state [%u] (doc: %s)"
                    , identity->state
                    , MENGINE_DOCUMENT_STR( identity->doc )
                );
#endif

                return false;
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::resumeEmitter( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr" );

            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        ESoundSourceState state = identity->getState();

        switch( state )
        {
        case ESS_PAUSE:
            {
                if( this->checkMaxSoundPlay_() == false )
                {
                    return false;
                }

                this->updateSourceVolume_( identity );

                const SoundSourceInterfacePtr & source = identity->getSoundSource();

                float duration = source->getDuration();
                float position = source->getPosition();

                identity->time_left = duration - position;

                identity->state = ESS_PLAY;

                if( identity->turn == true )
                {
                    if( source->resume() == false )
                    {
                        LOGGER_ERROR( "invalid play %u"
                            , identity->getId()
                        );

                        return false;
                    }

                    this->resumeSoundBufferUpdate_( identity );
                }

                const SoundListenerInterfacePtr & listener = identity->getSoundListener();

                if( listener != nullptr )
                {
                    SoundListenerInterfacePtr keep_listener = listener;
                    keep_listener->onSoundResume( identity );
                }
            }break;
        default:
            {
#ifdef MENGINE_DEBUG
                LOGGER_WARNING( "invalid state [%u] (doc: %s)"
                    , identity->state
                    , MENGINE_DOCUMENT_STR( identity->doc )
                );
#endif

                return false;
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::stopEmitter( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr" );

            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        ESoundSourceState state = identity->state;

        switch( state )
        {
        case ESS_PLAY:
        case ESS_PAUSE:
            {
                identity->state = ESS_STOP;

                if( identity->turn == true )
                {
                    this->stopSoundBufferUpdate_( identity );
                }

                const SoundSourceInterfacePtr & source = identity->getSoundSource();

                if( source != nullptr )
                {
                    float duration = source->getDuration();
                    float position = source->getPosition();

                    identity->time_left = duration - position;

                    source->stop();
                }

                if( identity->listener != nullptr )
                {
                    SoundListenerInterfacePtr keep_listener = identity->getSoundListener();
                    keep_listener->onSoundStop( identity );
                }
            }break;
        default:
            {
#ifdef MENGINE_DEBUG
                LOGGER_WARNING( "invalid state [%u] (doc: %s)"
                    , identity->state
                    , MENGINE_DOCUMENT_STR( identity->doc )
                );
#endif

                return false;
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::isEmitterStop( const SoundIdentityInterfacePtr & _identity ) const
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr" );

            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        bool stopped = identity->state == ESS_STOP;

        return stopped;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::isEmitterPlay( const SoundIdentityInterfacePtr & _identity ) const
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr" );

            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        bool played = identity->state == ESS_PLAY;

        return played;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::isEmitterPause( const SoundIdentityInterfacePtr & _identity ) const
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr" );

            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        bool paused = identity->state == ESS_PAUSE;

        return paused;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::setLoop( const SoundIdentityInterfacePtr & _identity, bool _looped )
    {
        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        identity->looped = _looped;

        const SoundSourceInterfacePtr & source = identity->getSoundSource();
        source->setLoop( _looped );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::getLoop( const SoundIdentityInterfacePtr & _identity ) const
    {
        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        bool looped = identity->getLoop();

        return looped;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::updateSoundVolumeProvider_( const SoundVolumeProviderInterfacePtr & _provider )
    {
        float commonVolume = m_commonVolume.mixValue();

        float soundVolume = m_soundVolume.mixValue();
        float musicVolume = m_musicVolume.mixValue();
        float voiceVolume = m_voiceVolume.mixValue();

        float mixSoundVolume = 1.f;
        mixSoundVolume *= commonVolume;
        mixSoundVolume *= soundVolume;

        float mixMusicVolume = 1.f;
        mixMusicVolume *= commonVolume;
        mixMusicVolume *= musicVolume;

        float mixVoiceVolume = 1.f;
        mixVoiceVolume *= commonVolume;
        mixVoiceVolume *= voiceVolume;

        _provider->onSoundChangeVolume( mixSoundVolume, mixMusicVolume, mixVoiceVolume, m_muted );
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::updateVolume()
    {
        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            this->updateSourceVolume_( identity );
        }

        for( const SoundVolumeProviderInterfacePtr & provider : m_soundVolumeProviders )
        {
            this->updateSoundVolumeProvider_( provider );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::notifyEnginePrepareFinalize()
    {
        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            this->stopSoundBufferUpdate_( identity );

            identity->finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::setSourceVolume( const SoundIdentityInterfacePtr & _identity, float _volume, float _default, bool _force )
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr (volume '%f' default '%f' force [%u])"
                , _volume
                , _default
                , _force
            );

            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        identity->volume.setValue( STRINGIZE_STRING_LOCAL( "General" ), _volume, _default, _force );

        this->updateSourceVolume_( identity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getSourceVolume( const SoundIdentityInterfacePtr & _identity ) const
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr" );

            return 0.f;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        float volume = identity->volume.mixValue();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::setSourceMixerVolume( const SoundIdentityInterfacePtr & _identity, const ConstString & _mixer, float _volume, float _default )
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr (mixer '%s' volume '%f' default '%f')"
                , _mixer.c_str()
                , _volume
                , _default
            );

            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        identity->volume.setValue( _mixer, _volume, _default, true );

        this->updateSourceVolume_( identity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getSourceMixerVolume( const SoundIdentityInterfacePtr & _identity, const ConstString & _mixer ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "identity is nullptr (mixer '%s')"
            , _mixer.c_str()
        );

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        float volume = identity->volume.getValue( _mixer );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getDuration( const SoundIdentityInterfacePtr & _identity ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "identity is nullptr" );

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        const SoundSourceInterfacePtr & source = identity->getSoundSource();

        if( source == nullptr )
        {
            LOGGER_ERROR( "source is nullptr id %u"
                , _identity->getId()
            );

            return 0.f;
        }

        float ms = source->getDuration();

        return ms;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::setPosMs( const SoundIdentityInterfacePtr & _identity, float _pos )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "identity is nullptr (pos '%f')"
            , _pos
        );

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        const SoundSourceInterfacePtr & source = identity->getSoundSource();

        if( source == nullptr )
        {
            LOGGER_ERROR( "not setup source %u"
                , identity->getId()
            );

            return false;
        }

        float duration = source->getDuration();

        if( _pos > duration )
        {
            LOGGER_ERROR( "emitter %u invalid position %f because length %f"
                , identity->getId()
                , _pos
                , duration
            );

            _pos = duration;
        }

        identity->time_left = duration - _pos;

        bool hasBufferUpdate = identity->worker != nullptr;

        if( hasBufferUpdate == true )
        {
            this->pauseSoundBufferUpdate_( identity );
        }

        float current_pos = source->getPosition();

        if( mt::equal_f_f( current_pos, _pos ) == true )
        {
            if( hasBufferUpdate == true )
            {
                this->resumeSoundBufferUpdate_( identity );
            }

            return true;
        }

        bool playing = source->isPlay();
        bool pausing = source->isPause();

        if( hasBufferUpdate == true && playing == true && pausing == false )
        {
            source->pause();
        }

        if( source->setPosition( _pos ) == false )
        {
            return false;
        }

        if( hasBufferUpdate == true && playing == true && pausing == false )
        {
            if( source->resume() == false )
            {
                LOGGER_ERROR( "invalid play" );

                return false;
            }
        }

        if( hasBufferUpdate == true )
        {
            this->resumeSoundBufferUpdate_( identity );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::stopSoundBufferUpdate_( const SoundIdentityPtr & _identity )
    {
        if( _identity->streamable == false )
        {
            return false;
        }

        if( _identity->worker == nullptr )
        {
            return false;
        }

        if( m_threadJobSoundBufferUpdate != nullptr )
        {
            m_threadJobSoundBufferUpdate->removeWorker( _identity->workerId );
        }

        _identity->worker = nullptr;
        _identity->workerId = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::playSoundBufferUpdate_( const SoundIdentityPtr & _identity )
    {
        if( _identity->streamable == false )
        {
            return false;
        }

        if( _identity->worker != nullptr )
        {
            LOGGER_ERROR( "identity worker is not null" );

            return false;
        }

        if( m_threadJobSoundBufferUpdate != nullptr )
        {
            ThreadWorkerSoundBufferUpdatePtr worker = m_factoryWorkerTaskSoundBufferUpdate->createObject( MENGINE_DOCUMENT_FACTORABLE );

            const SoundSourceInterfacePtr & source = _identity->getSoundSource();

            const SoundBufferInterfacePtr & soundBuffer = source->getSoundBuffer();

            worker->initialize( soundBuffer );

            _identity->worker = worker;

            UniqueId workerId = m_threadJobSoundBufferUpdate->addWorker( _identity->worker, MENGINE_DOCUMENT_FACTORABLE );

            if( workerId == INVALID_UNIQUE_ID )
            {
                LOGGER_ERROR( "identity worker invalid add worker" );

                return false;
            }

            _identity->workerId = workerId;
        }
        else
        {
            _identity->worker = nullptr;
            _identity->workerId = 0;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::pauseSoundBufferUpdate_( const SoundIdentityPtr & _identity )
    {
        if( _identity->streamable == false )
        {
            return false;
        }

        if( _identity->worker == nullptr )
        {
            return false;
        }

        if( m_threadJobSoundBufferUpdate != nullptr )
        {
            m_threadJobSoundBufferUpdate->pauseWorker( _identity->workerId );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::resumeSoundBufferUpdate_( const SoundIdentityPtr & _identity )
    {
        if( _identity->streamable == false )
        {
            return false;
        }

        if( _identity->worker == nullptr )
        {
            return false;
        }

        if( m_threadJobSoundBufferUpdate != nullptr )
        {
            m_threadJobSoundBufferUpdate->resumeWorker( _identity->workerId );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::checkMaxSoundPlay_() const
    {
        uint32_t playCount = (uint32_t)Algorithm::count_if( m_soundIdentities.begin(), m_soundIdentities.end(), []( const SoundIdentityPtr & _Identity )
        {
            return _Identity->state == ESS_PLAY;
        } );

        uint32_t Limit_MaxSoundPlay = CONFIG_VALUE( "Limit", "MaxSoundPlay", 16 );

        if( playCount > Limit_MaxSoundPlay )
        {
            LOGGER_ERROR( "Sound play exceeded max count '%u'"
                , Limit_MaxSoundPlay
            );

#ifdef MENGINE_DEBUG
            for( const SoundIdentityPtr & identity : m_soundIdentities )
            {
                if( identity->streamable == false )
                {
                    continue;
                }

                LOGGER_ERROR( "sound: %s [%s] "
                    , identity->streamable == true ? "streamable" : "instance"
                    , MENGINE_DOCUMENT_STR( identity->doc )
                );
            }
#endif

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getPosMs( const SoundIdentityInterfacePtr & _identity )
    {
        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        const SoundSourceInterfacePtr & source = identity->getSoundSource();

        if( source == nullptr )
        {
            return 0.f;
        }

        bool hasBufferUpdate = identity->worker != nullptr;

        if( hasBufferUpdate == true )
        {
            this->pauseSoundBufferUpdate_( identity );
        }

        float pos = source->getPosition();

        if( hasBufferUpdate == true )
        {
            this->resumeSoundBufferUpdate_( identity );
        }

        return pos;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::_stopService()
    {
        this->stopSounds_();

        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            this->stopSoundBufferUpdate_( identity );

            identity->finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

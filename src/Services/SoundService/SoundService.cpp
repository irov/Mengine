#include "SoundService.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/TimepipeServiceInterface.h"
#include "Interface/NotificatorInterface.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/PrefetcherHelper.h"
#include "Kernel/MixerBoolean.h"
#include "Kernel/MixerMultiplicative.h"
#include "Kernel/DebugFileHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdAlgorithm.h"

#include "math/utils.h"

#define SOUNDBUFFERUPDATE_PROCESSOR_NAME "SoundBufferUpdate"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundService, Mengine::SoundService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SoundService::SoundService()
        : m_supportStream( true )
        , m_turnStream( false )
        , m_turnSound( false )
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
            SERVICE_ID( ThreadServiceInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::_initializeService()
    {
        m_muted = PROTOTYPE_SERVICE()
            ->generatePrototype( ConstString::none(), MixerBoolean::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );

        m_commonVolume = PROTOTYPE_SERVICE()
            ->generatePrototype( ConstString::none(), MixerMultiplicative::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );

        m_soundVolume = PROTOTYPE_SERVICE()
            ->generatePrototype( ConstString::none(), MixerMultiplicative::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );

        m_musicVolume = PROTOTYPE_SERVICE()
            ->generatePrototype( ConstString::none(), MixerMultiplicative::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );

        m_voiceVolume = PROTOTYPE_SERVICE()
            ->generatePrototype( ConstString::none(), MixerMultiplicative::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );

        m_supportStream = THREAD_SERVICE()
            ->isAvailableService();

        if( m_supportStream == true )
        {
            ThreadJobPtr threadJobSoundBufferUpdate = THREAD_SERVICE()
                ->createJob( 25, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( threadJobSoundBufferUpdate, "invalid create thread job" );

            m_threadJobSoundBufferUpdate = threadJobSoundBufferUpdate;

            if( THREAD_SERVICE()
                ->createThreadProcessor( STRINGIZE_STRING_LOCAL_I( SOUNDBUFFERUPDATE_PROCESSOR_NAME ), MENGINE_THREAD_DESCRIPTION( "MNGSoundUpdate" ), ETP_NORMAL, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }

            if( THREAD_SERVICE()
                ->addTask( STRINGIZE_STRING_LOCAL_I( SOUNDBUFFERUPDATE_PROCESSOR_NAME ), m_threadJobSoundBufferUpdate, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }
        }

        float Engine_CommonVolume = CONFIG_VALUE_FLOAT( "Engine", "CommonVolume", 1.f );
        this->setCommonVolume( STRINGIZE_STRING_LOCAL( "Generic" ), Engine_CommonVolume, 0.f, MENGINE_MIXER_VALUE_DEFAULT_SPEED );

        float Engine_SoundVolume = CONFIG_VALUE_FLOAT( "Engine", "SoundVolume", 1.f );
        this->setSoundVolume( STRINGIZE_STRING_LOCAL( "Generic" ), Engine_SoundVolume, 0.f, MENGINE_MIXER_VALUE_DEFAULT_SPEED );

        float Engine_MusicVolume = CONFIG_VALUE_FLOAT( "Engine", "MusicVolume", 1.f );
        this->setMusicVolume( STRINGIZE_STRING_LOCAL( "Generic" ), Engine_MusicVolume, 0.f, MENGINE_MIXER_VALUE_DEFAULT_SPEED );

        float Engine_VoiceVolume = CONFIG_VALUE_FLOAT( "Engine", "VoiceVolume", 1.f );
        this->setVoiceVolume( STRINGIZE_STRING_LOCAL( "Generic" ), Engine_VoiceVolume, 0.f, MENGINE_MIXER_VALUE_DEFAULT_SPEED );

        bool OPTION_musicoff = HAS_OPTION( "musicoff" ) || HAS_OPTION( "nomusic" );

        if( OPTION_musicoff == true )
        {
            this->setMusicVolume( STRINGIZE_STRING_LOCAL( "__MusicOFF__" ), 0.f, 0.f, MENGINE_MIXER_VALUE_DEFAULT_SPEED );
        }

        m_factoryWorkerTaskSoundBufferUpdate = Helper::makeFactoryPoolWithMutex<ThreadWorkerSoundBufferUpdate, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factorySoundIdentity = Helper::makeFactoryPoolWithMutex<SoundIdentity, 32>( MENGINE_DOCUMENT_FACTORABLE );

        TIMEPIPE_SERVICE()
            ->addTimepipe( TimepipeInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE, &SoundService::notifyEnginePrepareFinalize, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );

        if( m_threadJobSoundBufferUpdate != nullptr )
        {
            THREAD_SERVICE()
                ->joinTask( m_threadJobSoundBufferUpdate );

            m_threadJobSoundBufferUpdate = nullptr;
        }

        if( m_supportStream == true )
        {
            THREAD_SERVICE()
                ->destroyThreadProcessor( STRINGIZE_STRING_LOCAL_I( SOUNDBUFFERUPDATE_PROCESSOR_NAME ) );
        }

        m_soundVolumeProviders.clear();

        TIMEPIPE_SERVICE()
            ->removeTimepipe( TimepipeInterfacePtr::from( this ) );

        m_muted = nullptr;

        m_commonVolume = nullptr;
        m_soundVolume = nullptr;
        m_musicVolume = nullptr;
        m_voiceVolume = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWorkerTaskSoundBufferUpdate );

        m_factorySoundIdentity = nullptr;
        m_factoryWorkerTaskSoundBufferUpdate = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::_stopService()
    {
        this->stopSounds_();

        for( const SoundIdentityInterfacePtr & identity : m_soundIdentities )
        {
            this->stopSoundBufferUpdate_( identity );

            identity->finalize();
        }

        m_soundIdentities.clear();
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
        VectorSoundVolumeProviders::iterator it_found = StdAlgorithm::find( m_soundVolumeProviders.begin(), m_soundVolumeProviders.end(), _soundVolumeProvider );

        if( it_found == m_soundVolumeProviders.end() )
        {
            return false;
        }

        m_soundVolumeProviders.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::pauseSounds_()
    {
        for( const SoundIdentityInterfacePtr & identity : m_soundIdentities )
        {
            identity->setTurn( false );

            ESoundSourceState state = identity->getState();

            if( state != ESS_PLAY )
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
        for( const SoundIdentityInterfacePtr & identity : m_soundIdentities )
        {
            identity->setTurn( true );

            ESoundSourceState state = identity->getState();

            if( state != ESS_PLAY )
            {
                continue;
            }

            this->updateSourceVolume_( identity );

            const SoundSourceInterfacePtr & source = identity->getSoundSource();

            if( source->isPlay() == true )
            {
                if( source->resume() == false )
                {
                    LOGGER_ASSERTION( "invalid resume (play)" );

                    continue;
                }

                this->resumeSoundBufferUpdate_( identity );
            }
            else
            {
                if( source->play() == false )
                {
                    LOGGER_ASSERTION( "invalid resume (stop)" );

                    continue;
                }

                this->playSoundBufferUpdate_( identity );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::stopSounds_()
    {
        for( const SoundIdentityInterfacePtr & identity : m_soundIdentities )
        {
            identity->setTurn( false );

            ESoundSourceState state = identity->getState();

            if( state == ESS_STOP )
            {
                continue;
            }

            if( state == ESS_CANCEL )
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
    SoundIdentityInterfacePtr SoundService::createSoundIdentity( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, ESoundSourceCategory _category, bool _streamable, const DocumentInterfacePtr & _doc )
    {
        if( m_supportStream == false && _streamable == true )
        {
            LOGGER_WARNING( "unsupport stream sound" );

            _streamable = false;
        }

        if( this->isStopService() == true )
        {
            LOGGER_ASSERTION( "service is stopped" );

            return nullptr;
        }

        SoundSourceInterfacePtr source = SOUND_SYSTEM()
            ->createSoundSource( _isHeadMode, _buffer, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( source, "create SoundSource invalid" );

        SoundIdentityInterfacePtr identity = m_factorySoundIdentity->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity, "create SoundIdentity invalid" );

        bool turn = _streamable == true ? m_turnStream : m_turnSound;

        LOGGER_INFO( "sound", "create sound identity: %u head: %d category: %d streamable: %d turn: %d source: %s"
            , identity->getUniqueIdentity()
            , _isHeadMode
            , _category
            , _streamable
            , turn
            , Helper::getDebugFullPath( _buffer->getDecoder()->getStream() ).c_str()
        );

        if( identity->initialize( source, _category, _streamable, turn ) == false )
        {
            LOGGER_ERROR( "invalide initialize identity sound" );

            return nullptr;
        }

        this->updateSourceVolume_( identity );

        m_soundIdentities.emplace_back( identity );

        return identity;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::updateSourceVolume_( const SoundIdentityInterfacePtr & _identity )
    {
        const SoundSourceInterfacePtr & source = _identity->getSoundSource();

        if( source == nullptr )
        {
            return;
        }

        bool streamable = _identity->getStreamable();

        bool mute = this->mixMute();

        if( mute == true ||
            (m_turnStream == false && streamable == true) ||
            (m_turnSound == false && streamable == false) )
        {
            source->setVolume( 0.f );
        }
        else
        {
            const MixerMultiplicativeInterfacePtr & emitterVolume = _identity->getMixerVolume();

            float mixVolume = 1.f;

            float commonMixVolume = m_commonVolume->mixValue();
            float emitterMixVolume = emitterVolume->mixValue();

            mixVolume *= commonMixVolume;
            mixVolume *= emitterMixVolume;

            ESoundSourceCategory type = _identity->getCategory();

            switch( type )
            {
            case ES_SOURCE_CATEGORY_SOUND:
                {
                    float soundMixVolume = m_soundVolume->mixValue();

                    mixVolume *= soundMixVolume;
                }break;
            case ES_SOURCE_CATEGORY_MUSIC:
                {
                    float musicMixVolume = m_musicVolume->mixValue();

                    mixVolume *= musicMixVolume;
                }break;
            case ES_SOURCE_CATEGORY_VOICE:
                {
                    float voiceMixVolume = m_voiceVolume->mixValue();

                    mixVolume *= voiceMixVolume;
                }break;
            }

            source->setVolume( mixVolume );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    SoundDecoderInterfacePtr SoundService::createSoundDecoder_( const ContentInterfacePtr & _content, bool _streamable, const DocumentInterfacePtr & _doc )
    {
        InputStreamInterfacePtr stream = _content->openInputStreamFile( _streamable, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't open sound file '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        const ConstString & codecType = _content->getCodecType();

        SoundDecoderInterfacePtr soundDecoder = CODEC_SERVICE()
            ->createDecoder( codecType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( soundDecoder, "can't create sound decoder for file '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        if( soundDecoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "can't initialize sound decoder for file '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return nullptr;
        }

        return soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr SoundService::createSoundBufferFromFile( const ContentInterfacePtr & _content, bool _streamable, const DocumentInterfacePtr & _doc )
    {
        if( this->isStopService() == true )
        {
            LOGGER_ASSERTION( "service is stopped" );

            return nullptr;
        }

        if( m_supportStream == false && _streamable == true )
        {
            LOGGER_WARNING( "unsupport stream sound '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            _streamable = false;
        }

        SoundDecoderInterfacePtr soundDecoder;
        if( _streamable == false )
        {
            if( Helper::getPrefetchSoundDecoder( _content, &soundDecoder ) == false )
            {
                soundDecoder = this->createSoundDecoder_( _content, false, _doc );
            }
            else
            {
                if( soundDecoder->rewind() == false )
                {
                    LOGGER_ERROR( "invalid rewind decoder '%s'"
                        , Helper::getContentFullPath( _content ).c_str()
                    );

                    return nullptr;
                }
            }
        }
        else
        {
            soundDecoder = this->createSoundDecoder_( _content, true, _doc );
        }

        if( soundDecoder == nullptr )
        {
            LOGGER_ERROR( "invalid create sound decoder '%s' codec '%s' streamable [%s]"
                , Helper::getContentFullPath( _content ).c_str()
                , _content->getCodecType().c_str()
                , _streamable == true ? "true" : "false"
            );

            return nullptr;
        }

        SoundBufferInterfacePtr buffer = SOUND_SYSTEM()
            ->createSoundBuffer( soundDecoder, _streamable, _doc );

        if( buffer == nullptr )
        {
            LOGGER_ERROR( "can't create sound buffer for file '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::releaseSoundIdentity( const SoundIdentityInterfacePtr & _identity )
    {
        this->stopSoundBufferUpdate_( _identity );

        _identity->finalize();

        VectorSoundIdentity::iterator it_found = StdAlgorithm::find( m_soundIdentities.begin(), m_soundIdentities.end(), _identity );

        if( it_found == m_soundIdentities.end() )
        {
            return false;
        }

        m_soundIdentities.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::foreachSoundIdentities( const LambdaSoundIdentity & _lambda ) const
    {
        for( const SoundIdentityInterfacePtr & identity : m_soundIdentities )
        {
            _lambda( identity );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::setSoundVolume( const ConstString & _type, float _volume, float _from, float _speed )
    {
        m_soundVolume->setValue( _type, _volume, _from, _speed, true );

        this->updateVolume();
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getSoundVolume( const ConstString & _type ) const
    {
        float volume = m_soundVolume->getValue( _type );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::mixSoundVolume() const
    {
        float volume = m_soundVolume->mixValue();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::setCommonVolume( const ConstString & _type, float _volume, float _from, float _speed )
    {
        m_commonVolume->setValue( _type, _volume, _from, _speed, true );

        this->updateVolume();
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getCommonVolume( const ConstString & _type ) const
    {
        float volume = m_commonVolume->getValue( _type );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::mixCommonVolume() const
    {
        float volume = m_commonVolume->mixValue();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::setMusicVolume( const ConstString & _type, float _volume, float _from, float _speed )
    {
        m_musicVolume->setValue( _type, _volume, _from, _speed, true );

        this->updateVolume();
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getMusicVolume( const ConstString & _type ) const
    {
        float volume = m_musicVolume->getValue( _type );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::mixMusicVolume() const
    {
        float volume = m_musicVolume->mixValue();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::setVoiceVolume( const ConstString & _type, float _volume, float _from, float _speed )
    {
        m_voiceVolume->setValue( _type, _volume, _from, _speed, true );

        this->updateVolume();
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getVoiceVolume( const ConstString & _type ) const
    {
        float volume = m_voiceVolume->getValue( _type );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::mixVoiceVolume() const
    {
        float volume = m_voiceVolume->mixValue();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::onTimepipe( const UpdateContext * _context )
    {
        SOUND_SYSTEM()
            ->update();

        bool process = false;

        if( m_commonVolume->update( _context ) == true )
        {
            process = true;
        }

        if( m_soundVolume->update( _context ) == true )
        {
            process = true;
        }

        if( m_musicVolume->update( _context ) == true )
        {
            process = true;
        }

        if( m_voiceVolume->update( _context ) == true )
        {
            process = true;
        }

        for( const SoundIdentityInterfacePtr & identity : m_soundIdentities )
        {
            ESoundSourceState state = identity->getState();

            if( state == ESS_CANCEL )
            {
                m_soundIdentitiesEndAux.emplace_back( identity );

                continue;
            }

            if( state != ESS_PLAY )
            {
                continue;
            }

            bool source_process = process;

            const MixerMultiplicativeInterfacePtr & mixerVolume = identity->getMixerVolume();

            if( mixerVolume->update( _context ) == true )
            {
                source_process = true;
            }

            if( source_process == true )
            {
                this->updateSourceVolume_( identity );
            }

            if( identity->getLoop() == true )
            {
                continue;
            }

            if( identity->getTurn() == false )
            {
                continue;
            }

            const ThreadWorkerSoundBufferUpdatePtr & worker = identity->getWorkerUpdateBuffer();

            float time_left = identity->getTimeLeft();
            float time_new = time_left - _context->time;

            if( worker != nullptr )
            {
                if( worker->isDone() == true )
                {
                    identity->setState( ESS_STOP );

                    this->stopSoundBufferUpdate_( identity );

                    const SoundSourceInterfacePtr & soundSource = identity->getSoundSource();

                    if( soundSource != nullptr )
                    {
                        soundSource->stop();
                    }

                    identity->setTimeLeft( 0.f );

                    m_soundIdentitiesEndAux.emplace_back( identity );
                }
                else
                {
                    if( time_new <= 0.f )
                    {
                        identity->setTimeLeft( 0.f );
                    }
                    else
                    {
                        identity->setTimeLeft( time_new );
                    }
                }
            }
            else
            {
                if( time_new <= 0.f )
                {
                    identity->setState( ESS_STOP );

                    const SoundSourceInterfacePtr & soundSource = identity->getSoundSource();

                    if( soundSource != nullptr )
                    {
                        soundSource->stop();
                    }

                    identity->setTimeLeft( 0.f );

                    m_soundIdentitiesEndAux.emplace_back( identity );
                }
                else
                {
                    identity->setTimeLeft( time_new );
                }
            }
        }

        if( process == true )
        {
            this->updateVolume();
        }

        for( const SoundIdentityInterfacePtr & identity : m_soundIdentitiesEndAux )
        {
            SoundListenerInterfacePtr listener = identity->popSoundListener();

            if( listener == nullptr )
            {
                continue;
            }

            listener->onSoundEnd( identity );
        }

        m_soundIdentitiesEndAux.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::setMute( const ConstString & _type, bool _mute )
    {
        m_muted->setValue( _type, _mute );

        this->updateVolume();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::getMute( const ConstString & _type ) const
    {
        bool value = m_muted->getValue( _type );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::mixMute() const
    {
        bool muted = m_muted->mixValue();

        return muted;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::playEmitter( const SoundIdentityInterfacePtr & _identity )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid play identity" );

        LOGGER_INFO( "sound", "play sound identity: %u state: %d category: %d streamable: %d turn: %d source: %s"
            , _identity->getUniqueIdentity()
            , _identity->getState()
            , _identity->getCategory()
            , _identity->getStreamable()
            , _identity->getTurn()
            , Helper::getDebugFullPath( _identity->getSoundSource()->getSoundBuffer()->getDecoder()->getStream() ).c_str()
        );

        ESoundSourceState state = _identity->getState();

        switch( state )
        {
        case ESS_STOP:
            {
                if( this->checkMaxSoundPlay_() == false )
                {
                    _identity->setState( ESS_CANCEL );

                    return false;
                }

                this->updateSourceVolume_( _identity );

                const SoundSourceInterfacePtr & source = _identity->getSoundSource();

                if( source == nullptr )
                {
                    LOGGER_ERROR( "invalid play identity: %u source is nullptr"
                        , _identity->getUniqueIdentity()
                    );

                    return false;
                }

                float duration = source->getDuration();
                float position = source->getPosition();

                float timeLeft = duration - position;

                _identity->setTimeLeft( timeLeft );

                _identity->setState( ESS_PLAY );

                if( _identity->getTurn() == true )
                {
                    if( source->play() == false )
                    {
                        LOGGER_ASSERTION( "invalid play identity: %u"
                            , _identity->getUniqueIdentity()
                        );

                        return false;
                    }

                    this->playSoundBufferUpdate_( _identity );
                }
            }break;
        case ESS_PAUSE:
            {
                if( this->checkMaxSoundPlay_() == false )
                {
                    _identity->setState( ESS_CANCEL );

                    return false;
                }

                this->updateSourceVolume_( _identity );

                const SoundSourceInterfacePtr & source = _identity->getSoundSource();

                float duration = source->getDuration();
                float position = source->getPosition();

                float timeLeft = duration - position;

                _identity->setTimeLeft( timeLeft );

                _identity->setState( ESS_PLAY );

                if( _identity->getTurn() == true )
                {
                    if( source->resume() == false )
                    {
                        LOGGER_ASSERTION( "invalid resume identity: %u"
                            , _identity->getUniqueIdentity()
                        );

                        return false;
                    }

                    this->resumeSoundBufferUpdate_( _identity );
                }

                const SoundListenerInterfacePtr & listener = _identity->getSoundListener();

                if( listener != nullptr )
                {
                    SoundListenerInterfacePtr keep_listener = listener;
                    keep_listener->onSoundResume( _identity );
                }
            }break;
        default:
            {
#if defined(MENGINE_DEBUG)
                LOGGER_WARNING( "invalid state [%u] (doc: %s)"
                    , _identity->getState()
                    , MENGINE_DOCUMENT_STR( _identity->getDocument() )
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
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid pause identity" );

        LOGGER_INFO( "sound", "pause sound identity: %u state: %d category: %d streamable: %d turn: %d source: %s"
            , _identity->getUniqueIdentity()
            , _identity->getState()
            , _identity->getCategory()
            , _identity->getStreamable()
            , _identity->getTurn()
            , Helper::getDebugFullPath( _identity->getSoundSource()->getSoundBuffer()->getDecoder()->getStream() ).c_str()
        );

        ESoundSourceState state = _identity->getState();

        switch( state )
        {
        case ESS_PLAY:
            {
                _identity->setState( ESS_PAUSE );

                if( _identity->getTurn() == true )
                {
                    this->pauseSoundBufferUpdate_( _identity );
                }

                const SoundSourceInterfacePtr & source = _identity->getSoundSource();

                source->pause();

                float duration = source->getDuration();
                float position = source->getPosition();

                float timeLeft = duration - position;

                _identity->setTimeLeft( timeLeft );

                const SoundListenerInterfacePtr & listener = _identity->getSoundListener();

                if( listener != nullptr )
                {
                    SoundListenerInterfacePtr keep_listener = listener;
                    keep_listener->onSoundPause( _identity );
                }
            }break;
        default:
            {
#if defined(MENGINE_DEBUG)
                LOGGER_WARNING( "invalid state [%u] (doc: %s)"
                    , _identity->getState()
                    , MENGINE_DOCUMENT_STR( _identity->getDocument() )
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
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid resume identity" );

        LOGGER_INFO( "sound", "resume sound identity: %u state: %d category: %d streamable: %d turn: %d source: %s"
            , _identity->getUniqueIdentity()
            , _identity->getState()
            , _identity->getCategory()
            , _identity->getStreamable()
            , _identity->getTurn()
            , Helper::getDebugFullPath( _identity->getSoundSource()->getSoundBuffer()->getDecoder()->getStream() ).c_str()
        );

        ESoundSourceState state = _identity->getState();

        switch( state )
        {
        case ESS_PAUSE:
            {
                if( this->checkMaxSoundPlay_() == false )
                {
                    return false;
                }

                this->updateSourceVolume_( _identity );

                const SoundSourceInterfacePtr & source = _identity->getSoundSource();

                float duration = source->getDuration();
                float position = source->getPosition();

                float timeLeft = duration - position;

                _identity->setTimeLeft( timeLeft );
                _identity->setState( ESS_PLAY );

                if( _identity->getTurn() == true )
                {
                    if( source->resume() == false )
                    {
                        LOGGER_ASSERTION( "invalid resume identity: %u"
                            , _identity->getUniqueIdentity()
                        );

                        return false;
                    }

                    this->resumeSoundBufferUpdate_( _identity );
                }

                const SoundListenerInterfacePtr & listener = _identity->getSoundListener();

                if( listener != nullptr )
                {
                    SoundListenerInterfacePtr keep_listener = listener;
                    keep_listener->onSoundResume( _identity );
                }
            }break;
        default:
            {
#if defined(MENGINE_DEBUG)
                LOGGER_WARNING( "invalid state [%u] (doc: %s)"
                    , _identity->getState()
                    , MENGINE_DOCUMENT_STR( _identity->getDocument() )
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
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid stop identity" );

        LOGGER_INFO( "sound", "stop sound identity: %u state: %d category: %d streamable: %d turn: %d source: %s"
            , _identity->getUniqueIdentity()
            , _identity->getState()
            , _identity->getCategory()
            , _identity->getStreamable()
            , _identity->getTurn()
            , Helper::getDebugFullPath( _identity->getSoundSource()->getSoundBuffer()->getDecoder()->getStream() ).c_str()
        );

        ESoundSourceState state = _identity->getState();

        switch( state )
        {
        case ESS_PLAY:
        case ESS_PAUSE:
            {
                _identity->setState( ESS_STOP );

                if( _identity->getTurn() == true )
                {
                    this->stopSoundBufferUpdate_( _identity );
                }

                const SoundSourceInterfacePtr & source = _identity->getSoundSource();

                if( source != nullptr )
                {
                    float duration = source->getDuration();
                    float position = source->getPosition();

                    float timeLeft = duration - position;
                    _identity->setTimeLeft( timeLeft );

                    source->stop();
                }

                if( _identity->getSoundListener() != nullptr )
                {
                    SoundListenerInterfacePtr keep_listener = _identity->getSoundListener();
                    keep_listener->onSoundStop( _identity );
                }
            }break;
        default:
            {
#if defined(MENGINE_DEBUG)
                LOGGER_WARNING( "invalid state [%u] (doc: %s)"
                    , _identity->getState()
                    , MENGINE_DOCUMENT_STR( _identity->getDocument() )
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
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid stop identity" );

        ESoundSourceState state = _identity->getState();

        if( state != ESS_STOP )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::isEmitterPlay( const SoundIdentityInterfacePtr & _identity ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid play identity" );

        ESoundSourceState state = _identity->getState();

        if( state != ESS_PLAY )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::isEmitterPause( const SoundIdentityInterfacePtr & _identity ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid pause identity" );

        ESoundSourceState state = _identity->getState();

        if( state != ESS_PAUSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::setLoop( const SoundIdentityInterfacePtr & _identity, bool _looped )
    {
        _identity->setLoop( _looped );

        const SoundSourceInterfacePtr & source = _identity->getSoundSource();
        source->setLoop( _looped );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::getLoop( const SoundIdentityInterfacePtr & _identity ) const
    {
        bool looped = _identity->getLoop();

        return looped;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::updateSoundVolumeProvider_( const SoundVolumeProviderInterfacePtr & _provider )
    {
        float commonVolume = m_commonVolume->mixValue();

        float soundVolume = m_soundVolume->mixValue();
        float musicVolume = m_musicVolume->mixValue();
        float voiceVolume = m_voiceVolume->mixValue();

        float mixSoundVolume = 1.f;
        mixSoundVolume *= commonVolume;
        mixSoundVolume *= soundVolume;

        float mixMusicVolume = 1.f;
        mixMusicVolume *= commonVolume;
        mixMusicVolume *= musicVolume;

        float mixVoiceVolume = 1.f;
        mixVoiceVolume *= commonVolume;
        mixVoiceVolume *= voiceVolume;

        bool mixMuted = m_muted->mixValue();

        _provider->onSoundChangeVolume( mixSoundVolume, mixMusicVolume, mixVoiceVolume, mixMuted );
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::updateVolume()
    {
        for( const SoundIdentityInterfacePtr & identity : m_soundIdentities )
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
        for( const SoundIdentityInterfacePtr & identity : m_soundIdentities )
        {
            this->stopSoundBufferUpdate_( identity );

            identity->finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::setSourceVolume( const SoundIdentityInterfacePtr & _identity, float _volume, float _from, float _speed, bool _force )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid identity set source volume" );

        const MixerMultiplicativeInterfacePtr & mixerVolume = _identity->getMixerVolume();

        mixerVolume->setValue( STRINGIZE_STRING_LOCAL( "General" ), _volume, _from, _speed, _force );

        this->updateSourceVolume_( _identity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getSourceVolume( const SoundIdentityInterfacePtr & _identity ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid identity get source volume" );

        const MixerMultiplicativeInterfacePtr & mixerVolume = _identity->getMixerVolume();

        float volume = mixerVolume->mixValue();

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::setSourceMixerVolume( const SoundIdentityInterfacePtr & _identity, const ConstString & _mixer, float _volume, float _from )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid identity set source mixer volume" );

        const MixerMultiplicativeInterfacePtr & mixerVolume = _identity->getMixerVolume();

        mixerVolume->setValue( _mixer, _volume, _from, MENGINE_MIXER_VALUE_DEFAULT_SPEED, true );

        this->updateSourceVolume_( _identity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getSourceMixerVolume( const SoundIdentityInterfacePtr & _identity, const ConstString & _mixer ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid identity get source mixer volume" );

        const MixerMultiplicativeInterfacePtr & mixerVolume = _identity->getMixerVolume();

        float volume = mixerVolume->getValue( _mixer );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getDuration( const SoundIdentityInterfacePtr & _identity ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid identity get duration" );

        const SoundSourceInterfacePtr & source = _identity->getSoundSource();

        if( source == nullptr )
        {
            LOGGER_ERROR( "source is nullptr identity: %u"
                , _identity->getUniqueIdentity()
            );

            return 0.f;
        }

        float ms = source->getDuration();

        return ms;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::setPosition( const SoundIdentityInterfacePtr & _identity, float _position )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid identity set pos ms" );

        const SoundSourceInterfacePtr & source = _identity->getSoundSource();

        if( source == nullptr )
        {
            LOGGER_ERROR( "not setup source identity: %u"
                , _identity->getUniqueIdentity()
            );

            return false;
        }

        float duration = source->getDuration();

        if( _position > duration )
        {
            LOGGER_ASSERTION( "emitter %u invalid position %f because length %f"
                , _identity->getUniqueIdentity()
                , _position
                , duration
            );

            _position = duration;
        }

        float timeLeft = duration - _position;
        _identity->setTimeLeft( timeLeft );

        bool hasBufferUpdate = _identity->getWorkerUpdateBuffer() != nullptr;

        if( hasBufferUpdate == true )
        {
            this->pauseSoundBufferUpdate_( _identity );
        }

        float current_position = source->getPosition();

        if( mt::equal_f_f( current_position, _position ) == true )
        {
            if( hasBufferUpdate == true )
            {
                this->resumeSoundBufferUpdate_( _identity );
            }

            return true;
        }

        bool playing = source->isPlay();
        bool pausing = source->isPause();

        if( hasBufferUpdate == true && playing == true && pausing == false )
        {
            source->pause();
        }

        if( source->setPosition( _position ) == false )
        {
            return false;
        }

        if( hasBufferUpdate == true && playing == true && pausing == false )
        {
            if( source->resume() == false )
            {
                LOGGER_ASSERTION( "invalid resume identity: %u"
                    , _identity->getUniqueIdentity()
                );

                return false;
            }
        }

        if( hasBufferUpdate == true )
        {
            this->resumeSoundBufferUpdate_( _identity );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::stopSoundBufferUpdate_( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity->getStreamable() == false )
        {
            return false;
        }

        if( _identity->getWorkerUpdateBuffer() == nullptr )
        {
            return false;
        }

        if( m_threadJobSoundBufferUpdate != nullptr )
        {
            UniqueId workerId = _identity->getWorkerId();

            m_threadJobSoundBufferUpdate->removeWorker( workerId );
        }

        _identity->setWorkerUpdateBuffer( nullptr );
        _identity->setWorkerId( INVALID_UNIQUE_ID );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::playSoundBufferUpdate_( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity->getStreamable() == false )
        {
            return false;
        }

        if( _identity->getWorkerUpdateBuffer() != nullptr )
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

            _identity->setWorkerUpdateBuffer( worker );

            UniqueId workerId = m_threadJobSoundBufferUpdate->addWorker( worker, MENGINE_DOCUMENT_FACTORABLE );

            if( workerId == INVALID_UNIQUE_ID )
            {
                LOGGER_ERROR( "identity worker invalid add worker" );

                return false;
            }

            _identity->setWorkerId( workerId );
        }
        else
        {
            _identity->setWorkerUpdateBuffer( nullptr );
            _identity->setWorkerId( INVALID_UNIQUE_ID );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::pauseSoundBufferUpdate_( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity->getStreamable() == false )
        {
            return false;
        }

        if( _identity->getWorkerUpdateBuffer() == nullptr )
        {
            return false;
        }

        if( m_threadJobSoundBufferUpdate != nullptr )
        {
            UniqueId workerId = _identity->getWorkerId();

            m_threadJobSoundBufferUpdate->pauseWorker( workerId );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::resumeSoundBufferUpdate_( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity->getStreamable() == false )
        {
            return false;
        }

        if( _identity->getWorkerUpdateBuffer() == nullptr )
        {
            return false;
        }

        if( m_threadJobSoundBufferUpdate != nullptr )
        {
            UniqueId workerId = _identity->getWorkerId();

            m_threadJobSoundBufferUpdate->resumeWorker( workerId );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::checkMaxSoundPlay_() const
    {
        uint32_t playCount = (uint32_t)StdAlgorithm::count_if( m_soundIdentities.begin(), m_soundIdentities.end(), []( const SoundIdentityInterfacePtr & _identity )
        {
            return _identity->getState() == ESS_PLAY;
        } );

        uint32_t Limit_MaxSoundPlay = CONFIG_VALUE_INTEGER( "Limit", "MaxSoundPlay", 32 );

        if( playCount > Limit_MaxSoundPlay )
        {
            Stringstream ss;
            ss << "max sound play count exceeded: " << Limit_MaxSoundPlay;

#if defined(MENGINE_DEBUG)
            Timestamp timestamp = Helper::getSystemTimestamp();

            for( const SoundIdentityInterfacePtr & identity : m_soundIdentities )
            {
                ss << "\n"
                    << "id: " << identity->getUniqueIdentity() << " "
                    << (identity->getStreamable() == true ? "streamable" : "instance") << " "
                    << "category: " << identity->getCategory() << " "
                    << "state: " << identity->getState() << " "
                    << "time: " << timestamp - identity->getStateTimestamp() << " "
                    << "source: " << Helper::getDebugFullPath( identity->getSoundSource()->getSoundBuffer()->getDecoder()->getStream() ).c_str() << " "
                    << "(doc: " << MENGINE_DOCUMENT_STR( identity->getDocument() ) << ")";
            }
#endif
            LOGGER_ASSERTION( "%s", ss.str().c_str() );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getPosition( const SoundIdentityInterfacePtr & _identity )
    {
        const SoundSourceInterfacePtr & source = _identity->getSoundSource();

        if( source == nullptr )
        {
            return 0.f;
        }

        bool hasBufferUpdate = _identity->getWorkerUpdateBuffer() != nullptr;

        if( hasBufferUpdate == true )
        {
            this->pauseSoundBufferUpdate_( _identity );
        }

        float position = source->getPosition();

        if( hasBufferUpdate == true )
        {
            this->resumeSoundBufferUpdate_( _identity );
        }

        return position;
    }
    //////////////////////////////////////////////////////////////////////////
}

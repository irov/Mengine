#include "SoundService.h"

#include "Interface/SoundCodecInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "math/utils.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundService, Mengine::SoundService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SoundService::SoundService()
        : m_supportStream( true )
        , m_muted( false )
        , m_turnStream( false )
        , m_turnSound( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundService::~SoundService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::_initializeService()
    {
        m_supportStream = THREAD_SERVICE()
            ->isAvailableService();

        if( m_supportStream == true )
        {
            m_threadJobSoundBufferUpdate = THREAD_SERVICE()
                ->createJob( 25, MENGINE_DOCUMENT_FUNCTION );

            THREAD_SERVICE()
                ->createThread( STRINGIZE_STRING_LOCAL( "ThreadSoundBufferUpdate" ), 0, MENGINE_DOCUMENT_FUNCTION );

            THREAD_SERVICE()
                ->addTask( STRINGIZE_STRING_LOCAL( "ThreadSoundBufferUpdate" ), m_threadJobSoundBufferUpdate );
        }

        float commonVolume = CONFIG_VALUE( "Engine", "CommonVolume", 1.f );
        this->setCommonVolume( STRINGIZE_STRING_LOCAL( "Generic" ), commonVolume, 0.f );

        float soundVolume = CONFIG_VALUE( "Engine", "SoundVolume", 1.f );
        this->setSoundVolume( STRINGIZE_STRING_LOCAL( "Generic" ), soundVolume, 0.f );

        float musicVolume = CONFIG_VALUE( "Engine", "MusicVolume", 1.f );
        this->setMusicVolume( STRINGIZE_STRING_LOCAL( "Generic" ), musicVolume, 0.f );

        float voiceVolume = CONFIG_VALUE( "Engine", "VoiceVolume", 1.f );
        this->setVoiceVolume( STRINGIZE_STRING_LOCAL( "Generic" ), voiceVolume, 0.f );

        bool musicoff = GET_OPTION_VALUE( "musicoff" ) || GET_OPTION_VALUE( "nomusic" );

        if( musicoff == true )
        {
            this->setMusicVolume( STRINGIZE_STRING_LOCAL( "__MusicOFF__" ), 0.f, 0.f );
        }

        m_factoryWorkerTaskSoundBufferUpdate = new FactoryPool<ThreadWorkerSoundBufferUpdate, 32>();
        m_factorySoundEmitter = new FactoryPool<SoundIdentity, 32>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::_finalizeService()
    {
        this->stopSounds_();

        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            if( identity == nullptr )
            {
                continue;
            }

            this->stopSoundBufferUpdate_( identity );

            const SoundSourceInterfacePtr & source = identity->getSoundSource();

            if( source != nullptr )
            {
                source->stop();

                identity->setSoundSource( nullptr );
            }
        }

        m_soundIdentities.clear();
        m_soundStopListeners.clear();

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

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundEmitter );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWorkerTaskSoundBufferUpdate );

        m_factorySoundEmitter = nullptr;
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
        VectorSoundVolumeProviders::iterator it_found = std::find( m_soundVolumeProviders.begin(), m_soundVolumeProviders.end(), _soundVolumeProvider );

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
            if( identity == nullptr )
            {
                continue;
            }

            identity->turn = true;

            if( identity->state != ESS_PLAY )
            {
                continue;
            }

            this->updateSourceVolume_( identity );

            if( identity->source->play() == false )
            {
                LOGGER_ERROR( "invalid play"
                );

                continue;
            }

            this->playSoundBufferUpdate_( identity );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::pauseSounds_()
    {
        for( const SoundIdentityPtr & source : m_soundIdentities )
        {
            if( source == nullptr )
            {
                continue;
            }

            source->turn = false;

            if( source->state != ESS_PLAY )
            {
                continue;
            }

            this->pauseSoundBufferUpdate_( source );

            source->source->pause();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::resumeSounds_()
    {
        for( const SoundIdentityPtr & identity : m_soundIdentities )
        {
            if( identity == nullptr )
            {
                continue;
            }

            identity->turn = true;

            if( identity->state != ESS_PLAY )
            {
                continue;
            }

            this->updateSourceVolume_( identity );

            if( identity->source->isPlay() == true )
            {
                if( identity->source->resume() == false )
                {
                    LOGGER_ERROR( "invalid resume (play)"
                    );

                    continue;
                }

                this->resumeSoundBufferUpdate_( identity );
            }
            else
            {
                if( identity->source->play() == false )
                {
                    LOGGER_ERROR( "invalid resume (stop)"
                    );

                    continue;
                }

                this->playSoundBufferUpdate_( identity );
            }            
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundService::stopSounds_()
    {
        for( const SoundIdentityPtr & source : m_soundIdentities )
        {
            if( source == nullptr )
            {
                continue;
            }

            source->turn = false;

            if( source->state != ESS_PLAY )
            {
                continue;
            }

            this->stopSoundBufferUpdate_( source );

            if( source->source != nullptr )
            {
                source->source->stop();
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
    SoundIdentityInterfacePtr SoundService::createSoundIdentity( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, ESoundSourceCategory _category, bool _streamable, const Char * _doc )
    {
        if( m_supportStream == false && _streamable == true )
        {
            LOGGER_WARNING( "unsupport stream sound"
            );

            _streamable = false;
        }

        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stoped"
            );

            return nullptr;
        }

        SoundSourceInterfacePtr sourceInterface = SOUND_SYSTEM()
            ->createSoundSource( _isHeadMode, _buffer );

        if( sourceInterface == nullptr )
        {
            LOGGER_ERROR( "create SoundSource invalid"
            );

            return nullptr;
        }

		SoundIdentityPtr emitter = m_factorySoundEmitter->createObject( _doc );

        uint32_t new_id = GENERATE_UNIQUE_IDENTITY();
        emitter->id = new_id;

        emitter->source = sourceInterface;
        emitter->listener = nullptr;
        emitter->worker = nullptr;
        emitter->bufferId = 0;

        emitter->time = 0.f;

        emitter->state = ESS_STOP;
        emitter->category = _category;

        emitter->streamable = _streamable;
        emitter->looped = false;
        emitter->turn = _streamable ? m_turnStream : m_turnSound;

#ifdef MENGINE_DEBUG
        emitter->doc = _doc;
#endif

        this->updateSourceVolume_( emitter );

        m_soundIdentities.emplace_back( emitter );

        return emitter;
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

        if( this->isMute() == true ||
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
    SoundDecoderInterfacePtr SoundService::createSoundDecoder_( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, const ConstString & _codecType, bool _streamable )
    {
        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( _fileGroup, _filePath, _streamable, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "Can't open sound file %s:%s"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        SoundDecoderInterfacePtr soundDecoder = CODEC_SERVICE()
            ->createDecoderT<SoundDecoderInterfacePtr>( _codecType, MENGINE_DOCUMENT_FUNCTION );

        if( soundDecoder == nullptr )
        {
            LOGGER_ERROR( "Can't create sound decoder for file %s:%s"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        if( soundDecoder->prepareData( stream ) == false )
        {
            LOGGER_ERROR( "Can't initialize sound decoder for file %s:%s"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        return soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr SoundService::createSoundBufferFromFile( const FileGroupInterfacePtr& _fileGroup, const FilePath & _filePath, const ConstString & _codecType, bool _streamable )
    {
        if( m_supportStream == false && _streamable == true )
        {
            LOGGER_WARNING( "unsupport stream sound %s:%s"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            _streamable = false;
        }

        if( this->isStopService() == true )
        {
            LOGGER_ERROR( "service is stoped"
            );

            return nullptr;
        }

        SoundDecoderInterfacePtr soundDecoder;
        if( _streamable == false )
        {
            if( PREFETCHER_SERVICE()
                ->getSoundDecoder( _fileGroup, _filePath, soundDecoder ) == false )
            {
                soundDecoder = this->createSoundDecoder_( _fileGroup, _filePath, _codecType, false );
            }
            else
            {
                if( soundDecoder->rewind() == false )
                {
                    LOGGER_ERROR( "invalid rewind decoder '%s':'%s'"
                        , _fileGroup->getName().c_str()
                        , _filePath.c_str()
                    );

                    return nullptr;
                }
            }
        }
        else
        {
            soundDecoder = this->createSoundDecoder_( _fileGroup, _filePath, _codecType, true );
        }

        if( soundDecoder == nullptr )
        {
            LOGGER_ERROR( "invalid create decoder '%s':'%s' type %s"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
                , _codecType.c_str()
            );

            return nullptr;
        }

        SoundBufferInterfacePtr buffer = SOUND_SYSTEM()
            ->createSoundBuffer( soundDecoder, _streamable );

        if( buffer == nullptr )
        {
            LOGGER_ERROR( "Can't create sound buffer for file %s:%s"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::releaseSoundSource( const SoundIdentityInterfacePtr & _element )
    {
        this->stopSoundBufferUpdate_( _element );

        const SoundSourceInterfacePtr & source = _element->getSoundSource();

        if( source != nullptr )
        {
            source->stop();
            _element->setSoundSource( nullptr );
        }

        _element->setSoundListener( nullptr );

        uint32_t id = _element->getId();

        for( VectorSoundSource::iterator
            it = m_soundIdentities.begin(),
            it_end = m_soundIdentities.end();
            it != it_end;
            ++it )
        {
            const SoundIdentityPtr & identity = *it;

            if( identity->id != id )
            {
                continue;
            }

            m_soundIdentities.erase( it );

            break;
        }

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
    void SoundService::tick( const UpdateContext * _context )
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
            if( identity == nullptr )
            {
                continue;
            }

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

            float identity_timing = identity->time;

            if( (worker != nullptr && worker->isDone() == true) || (worker == nullptr && identity_timing <= 0.f) )
            {
                identity->state = ESS_STOP;
                this->stopSoundBufferUpdate_( identity );

                const SoundSourceInterfacePtr & soundSource = identity->getSoundSource();

                if( soundSource != nullptr )
                {
                    soundSource->stop();
                }

                identity->time = 0.f;

                if( identity->listener != nullptr )
                {
                    m_soundStopListeners.emplace_back( identity );
                }
            }
            else
            {
                identity->time -= _context->time;
            }
        }

        if( process == true )
        {
            this->updateVolume();
        }

        for( const SoundIdentityPtr & identity : m_soundStopListeners )
        {
            SoundListenerInterfacePtr keep_listener = identity->getSoundListener();
            keep_listener->onSoundEnd( identity );
        }

        m_soundStopListeners.clear();
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
            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        ESoundSourceState state = identity->getState();

        switch( state )
        {
        case ESS_STOP:
        case ESS_PAUSE:
            {
                if( this->checkMaxSoundPlay_() == false )
                {
                    return false;
                }

                this->updateSourceVolume_( identity );

                this->stopSoundBufferUpdate_( identity );

                const SoundSourceInterfacePtr & source = identity->getSoundSource();

                if( source == nullptr )
                {
                    LOGGER_ERROR( "invalid play %d source is nullptr"
                        , identity->id
                    );

                    return false;
                }

                float length_ms = source->getDuration();
                float pos_ms = source->getPosition();

                identity->time = length_ms - pos_ms;

                identity->state = ESS_PLAY;

                if( identity->turn == true )
                {
                    if( source->play() == false )
                    {
                        LOGGER_ERROR( "invalid play %d"
                            , identity->id
                        );

                        return false;
                    }

                    this->playSoundBufferUpdate_( identity );
                }
            }break;
        default:
            {
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::pauseEmitter( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity == nullptr )
        {
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
                float pos_ms = source->getPosition();

                identity->time = pos_ms;

                const SoundListenerInterfacePtr & listener = identity->getSoundListener();

                if( listener != nullptr )
                {
                    SoundListenerInterfacePtr keep_listener = listener;
                    keep_listener->onSoundPause( identity );
                }
            }break;
        default:
            {
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::resumeEmitter( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity == nullptr )
        {
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

                float length_ms = source->getDuration();
                float pos_ms = source->getPosition();

                identity->time = length_ms - pos_ms;

                identity->state = ESS_PLAY;

                if( identity->turn == true )
                {
                    if( source->resume() == false )
                    {
                        LOGGER_ERROR( "invalid play %d"
                            , identity->id
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
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::stopEmitter( const SoundIdentityInterfacePtr & _identity )
    {
        if( _identity == nullptr )
        {
            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        switch( identity->state )
        {
        case ESS_PLAY:
        case ESS_PAUSE:
            {
                identity->state = ESS_STOP;

                if( identity->turn == true )
                {
                    this->stopSoundBufferUpdate_( identity );
                }

                if( identity->source != nullptr )
                {
                    SoundSourceInterfacePtr source = identity->source;

                    identity->time = source->getPosition();
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
            }break;
        }

        return true;
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
            if( identity == nullptr )
            {
                continue;
            }

            this->updateSourceVolume_( identity );
        }

        for( const SoundVolumeProviderInterfacePtr & provider : m_soundVolumeProviders )
        {
            this->updateSoundVolumeProvider_( provider );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::setSourceVolume( const SoundIdentityInterfacePtr & _identity, float _volume, float _default, bool _force )
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr (volume '%f' default '%f' force '%d')"
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
            LOGGER_ERROR( "identity is nullptr"
            );

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
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr (mixer '%s')"
                , _mixer.c_str()
            );

            return 0.f;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        float volume = identity->volume.getValue( _mixer );

        return volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundService::getDuration( const SoundIdentityInterfacePtr & _identity ) const
    {
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr"
            );

            return 0.f;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        const SoundSourceInterfacePtr & source = identity->getSoundSource();

        if( source == nullptr )
        {
            LOGGER_ERROR( "source is nullptr id '%d'"
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
        if( _identity == nullptr )
        {
            LOGGER_ERROR( "identity is nullptr (pos '%f')"
                , _pos
            );

            return false;
        }

        SoundIdentityPtr identity = stdex::intrusive_static_cast<SoundIdentityPtr>(_identity);

        const SoundSourceInterfacePtr & source = identity->getSoundSource();

        if( source == nullptr )
        {
            LOGGER_ERROR( "not setup source %d"
                , identity->id
            );

            return false;
        }

        float lengthMs = source->getDuration();

        if( _pos > lengthMs )
        {
            LOGGER_ERROR( "emitter %d pos %f length %f"
                , identity->id
                , _pos
                , lengthMs
            );

            _pos = lengthMs;
        }

        identity->time = lengthMs - _pos;

        bool hasBufferUpdate = identity->worker != nullptr;

        if( hasBufferUpdate == true )
        {
            this->stopSoundBufferUpdate_( identity );
        }

        float current_pos = source->getPosition();

        if( mt::equal_f_f( current_pos, _pos ) == true )
        {
            if( hasBufferUpdate == true )
            {
                this->playSoundBufferUpdate_( identity );
            }

            return true;
        }

        bool playing = source->isPlay();
        bool pausing = source->isPause();

        if( playing == true && pausing == false )
        {
            source->pause();
        }

        if( source->setPosition( _pos ) == false )
        {
            return false;
        }

        if( playing == true && pausing == false )
        {
            this->updateSourceVolume_( identity );

            if( source->play() == false )
            {
                LOGGER_ERROR( "invalid play"
                );

                return false;
            }
        }

        if( hasBufferUpdate == true )
        {
            this->playSoundBufferUpdate_( identity );
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
            m_threadJobSoundBufferUpdate->removeWorker( _identity->bufferId );
        }

        _identity->worker = nullptr;
        _identity->bufferId = 0;

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
            LOGGER_ERROR( "identity worker is not null"
            );

            return false;
        }

        if( m_threadJobSoundBufferUpdate != nullptr )
        {
			ThreadWorkerSoundBufferUpdatePtr worker = m_factoryWorkerTaskSoundBufferUpdate->createObject( MENGINE_DOCUMENT_FUNCTION );

            const SoundBufferInterfacePtr & soundBuffer = _identity->source->getSoundBuffer();

            worker->initialize( soundBuffer );

            _identity->worker = worker;

            uint32_t bufferId = m_threadJobSoundBufferUpdate->addWorker( _identity->worker );

            if( bufferId == 0 )
            {
                LOGGER_ERROR( "identity worker invalid add worker"
                );

                return false;
            }

            _identity->bufferId = bufferId;
        }
        else
        {
            _identity->worker = nullptr;
            _identity->bufferId = 0;
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
            m_threadJobSoundBufferUpdate->pauseWorker( _identity->bufferId );
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
            m_threadJobSoundBufferUpdate->resumeWorker( _identity->bufferId );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundService::checkMaxSoundPlay_() const
    {
        uint32_t playCount = std::count_if( m_soundIdentities.begin(), m_soundIdentities.end(), []( const SoundIdentityPtr & _Identity ) { return _Identity->state == ESS_PLAY; } );

        uint32_t MaxSoundPlay = CONFIG_VALUE( "Limit", "MaxSoundPlay", 16 );

        if( playCount > MaxSoundPlay )
        {
            LOGGER_ERROR( "Sound play exceeded max count '%d'"
                , MaxSoundPlay
            );

#ifdef MENGINE_DEBUG
            for( const SoundIdentityPtr & identity : m_soundIdentities )
            {
                if( identity->streamable == false )
                {
                    continue;
                }

                LOGGER_ERROR( "sound: %s %s "
                    , identity->streamable == true ? "streamable" : "instance"
                    , identity->doc.c_str()
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

        VectorSoundSource remove_soundIdentities = m_soundIdentities;

        for( const SoundIdentityPtr & identity : remove_soundIdentities )
        {
            this->stopSoundBufferUpdate_( identity );

            if( identity->source != nullptr )
            {
                identity->source->stop();
                identity->source = nullptr;
            }

            if( identity->listener != nullptr )
            {
                SoundListenerInterfacePtr keep_identity = identity->listener;
                keep_identity->onSoundStop( identity );

                identity->listener = nullptr;
            }
        }
    }
}

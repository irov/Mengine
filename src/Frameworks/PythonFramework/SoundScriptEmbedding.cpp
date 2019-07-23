#include "SoundScriptEmbedding.h"

#include "Interface/AmplifierInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Affectorable.h"
#include "Kernel/AffectorHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionResourceType.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Frameworks/PythonFramework/DocumentTraceback.h"

#include "Config/Lambda.h"

#include "Engine/ResourceSound.h"

#include "pybind/pybind.hpp"

#include <math.h>

namespace Mengine
{
    class SoundScriptMethod
        : public Factorable
    {
    public:
        SoundScriptMethod()
            : m_affectorMusicID( 0 )
        {
            m_factorySoundAffectorCallback = new FactoryPool<SoundAffectorCallback, 4>();
            m_factoryMusicAffectorCallback = new FactoryPool<MusicAffectorCallback, 4>();
        }

        ~SoundScriptMethod() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundAffectorCallback );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMusicAffectorCallback );

            m_factorySoundAffectorCallback = nullptr;
            m_factoryMusicAffectorCallback = nullptr;
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        class MySoundNodeListener
            : public Factorable
            , public SoundListenerInterface
        {
        public:
            MySoundNodeListener( const ResourceSoundPtr & _resource, const SoundBufferInterfacePtr & _soundBuffer, const pybind::object & _cb, const pybind::args & _args )
                : m_resource( _resource )
                , m_soundBuffer( _soundBuffer )
                , m_cb( _cb )
                , m_args( _args )
            {
            }

            ~MySoundNodeListener() override
            {
                if( m_resource != nullptr )
                {
                    m_resource->release();
                    m_resource = nullptr;
                }
            }

        protected:
            void onSoundPause( const SoundIdentityInterfacePtr & _emitter ) override
            {
                MENGINE_UNUSED( _emitter );

                //Empty
            }

            void onSoundResume( const SoundIdentityInterfacePtr & _emitter ) override
            {
                MENGINE_UNUSED( _emitter );

                //Empty
            }

            void onSoundStop( const SoundIdentityInterfacePtr & _emitter ) override
            {
                if( m_cb.is_callable() == true )
                {
                    m_cb.call_args( _emitter, 0, m_args );
                }

                if( SOUND_SERVICE()
                    ->releaseSoundSource( _emitter ) == false )
                {
                    uint32_t id = _emitter->getId();

                    LOGGER_ERROR( "'%s' emitter invalid release sound '%d'"
                        , m_resource->getName().c_str()
                        , id
                    );
                }

                m_resource->release();
                m_resource = nullptr;

                m_soundBuffer = nullptr;
            }

            void onSoundEnd( const SoundIdentityInterfacePtr & _emitter ) override
            {
                if( m_cb.is_callable() == true )
                {
                    m_cb.call_args( _emitter, 1, m_args );
                }

                if( SOUND_SERVICE()
                    ->releaseSoundSource( _emitter ) == false )
                {
                    uint32_t id = _emitter->getId();

                    LOGGER_ERROR( "'%s' emitter invalid release sound '%d'"
                        , m_resource->getName().c_str()
                        , id
                    );
                }

                m_resource->release();
                m_resource = nullptr;

                m_soundBuffer = nullptr;
            }

        protected:
            ResourceSoundPtr m_resource;
            SoundBufferInterfacePtr m_soundBuffer;
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<MySoundNodeListener> MySoundNodeListenerPtr;
        //////////////////////////////////////////////////////////////////////////
        bool hasSound( const ConstString & _resourceName )
        {
            if( RESOURCE_SERVICE()
                ->hasResourceWithType( _resourceName, STRINGIZE_STRING_LOCAL( "ResourceSound" ) ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factorySoundNodeListener;
        //////////////////////////////////////////////////////////////////////////
        SoundIdentityInterfacePtr s_createSoundSource( const ConstString & _resourceName, bool _loop, ESoundSourceCategory _category, const pybind::object & _cb, const pybind::args & _args )
        {
            MENGINE_ASSERTION_RESOURCE_TYPE( _resourceName, ResourceSound *, nullptr, "createSoundSource resource '%s' type does not match 'ResourceSound'"
                , _resourceName.c_str()
            );

            const ResourceSoundPtr & resource = RESOURCE_SERVICE()
                ->getResource( _resourceName );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, nullptr );

            SoundBufferInterfacePtr soundBuffer = resource->createSoundBuffer();

            if( soundBuffer == nullptr )
            {
                resource->release();

                return nullptr;
            }

            bool streamable = resource->isStreamable();

            SoundIdentityInterfacePtr sourceEmitter = SOUND_SERVICE()
                ->createSoundIdentity( true, soundBuffer, _category, streamable
                    , MENGINE_DOCUMENT_PYBIND
                );

            if( sourceEmitter == nullptr )
            {
                LOGGER_ERROR( "sound '%s' invalid create identity"
                    , _resourceName.c_str()
                );

                if( _cb.is_callable() == true )
                {
                    _cb.call_args( nullptr, 2, _args );
                }

                resource->release();

                return nullptr;
            }

            SOUND_SERVICE()
                ->setLoop( sourceEmitter, _loop );

            float volume = resource->getDefaultVolume();

            if( SOUND_SERVICE()
                ->setSourceVolume( sourceEmitter, volume, volume, true ) == false )
            {
                LOGGER_ERROR( "sound '%s' invalid set volume %f"
                    , _resourceName.c_str()
                    , volume
                );

                if( _cb.is_callable() == true )
                {
                    _cb.call_args( nullptr, 2, _args );
                }

                resource->release();

                return nullptr;
            }

            MySoundNodeListenerPtr snlistener = Helper::makeFactorableUnique<MySoundNodeListener>( resource, soundBuffer, _cb, _args );

            sourceEmitter->setSoundListener( snlistener );

            return sourceEmitter;
        }
        //////////////////////////////////////////////////////////////////////////
        SoundIdentityInterfacePtr soundPlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb, const pybind::args & _args )
        {
            SoundIdentityInterfacePtr sourceEmitter = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cb, _args );

            MENGINE_ASSERTION_MEMORY_PANIC( sourceEmitter, nullptr, "can't get resource '%s'"
                , _resourceName.c_str()
            );

            if( SOUND_SERVICE()
                ->playEmitter( sourceEmitter ) == false )
            {
                LOGGER_ERROR( "invalid play '%s'"
                    , _resourceName.c_str()
                );

                return nullptr;
            }

            return sourceEmitter;
        }
        //////////////////////////////////////////////////////////////////////////
        SoundIdentityInterfacePtr voicePlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb, const pybind::args & _args )
        {
            SoundIdentityInterfacePtr sourceEmitter = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_VOICE, _cb, _args );

            MENGINE_ASSERTION_MEMORY_PANIC( sourceEmitter, nullptr, "can't get resource '%s'"
                , _resourceName.c_str()
            );

            if( SOUND_SERVICE()
                ->playEmitter( sourceEmitter ) == false )
            {
                LOGGER_ERROR( "invalid play '%s'"
                    , _resourceName.c_str()
                );

                return nullptr;
            }

            return sourceEmitter;
        }
        //////////////////////////////////////////////////////////////////////////
        bool voicePause( const SoundIdentityInterfacePtr & _emitter )
        {
            bool successful = SOUND_SERVICE()
                ->pauseEmitter( _emitter );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool voiceResume( const SoundIdentityInterfacePtr & _emitter )
        {
            bool successful = SOUND_SERVICE()
                ->resumeEmitter( _emitter );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        SoundIdentityInterfacePtr soundPlayFromPosition( const ConstString & _resourceName, float _position, bool _loop, const pybind::object & _cb, const pybind::args & _args )
        {
            SoundIdentityInterfacePtr sourceEmitter = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cb, _args );

            MENGINE_ASSERTION_MEMORY_PANIC( sourceEmitter, nullptr, "can't get resource '%s'"
                , _resourceName.c_str()
            );

            if( SOUND_SERVICE()
                ->setPosMs( sourceEmitter, _position ) == false )
            {
                LOGGER_ERROR( "set pos '%s' '%f'"
                    , _resourceName.c_str()
                    , _position
                );

                return nullptr;
            }

            if( SOUND_SERVICE()
                ->playEmitter( sourceEmitter ) == false )
            {
                LOGGER_ERROR( "play '%s' '%f'"
                    , _resourceName.c_str()
                    , _position
                );

                return nullptr;
            }

            return sourceEmitter;
        }
        //////////////////////////////////////////////////////////////////////////
        float soundGetPosMs( const SoundIdentityInterfacePtr & _emitter )
        {
            float pos = SOUND_SERVICE()
                ->getPosMs( _emitter );

            return pos;
        }
        //////////////////////////////////////////////////////////////////////////
        void soundSetPosMs( const SoundIdentityInterfacePtr & _emitter, float _pos )
        {
            SOUND_SERVICE()
                ->setPosMs( _emitter, _pos );
        }
        //////////////////////////////////////////////////////////////////////////
        class SoundAffectorCallback
            : public AffectorCallbackInterface
        {
        public:
            SoundAffectorCallback()
            {
            }

            ~SoundAffectorCallback() override
            {
            }

        public:
            void initialize( const SoundIdentityInterfacePtr & _emitter, const pybind::object & _cb, const pybind::args & _args )
            {
                m_emitter = _emitter;
                m_cb = _cb;
                m_args = _args;
            }

        protected:
            void onAffectorEnd( uint32_t _id, bool _isEnd ) override
            {
                if( _isEnd == true )
                {
                    SOUND_SERVICE()
                        ->stopEmitter( m_emitter );
                }

                if( m_cb.is_invalid() == true )
                {
                    return;
                }

                if( m_cb.is_none() == true )
                {
                    return;
                }

                m_cb.call_args( _id, _isEnd, m_args );
            }

        protected:
            SoundIdentityInterfacePtr m_emitter;
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<SoundAffectorCallback> SoundAffectorCallbackPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factorySoundAffectorCallback;
        //////////////////////////////////////////////////////////////////////////
        SoundAffectorCallbackPtr createSoundAffectorCallback( const SoundIdentityInterfacePtr & _emitter, const pybind::object & _cb, const pybind::args & _args )
        {
            SoundAffectorCallbackPtr callback = m_factorySoundAffectorCallback->createObject( MENGINE_DOCUMENT_PYBIND );
            callback->initialize( _emitter, _cb, _args );

            return callback;
        }
        //////////////////////////////////////////////////////////////////////////
        void ___soundFade( const SoundIdentityInterfacePtr & _emitter, float _volume )
        {
            SOUND_SERVICE()
                ->setSourceMixerVolume( _emitter, STRINGIZE_STRING_LOCAL( "Fade" ), _volume, _volume );
        }
        //////////////////////////////////////////////////////////////////////////		
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float> m_affectorCreatorSound;
        //////////////////////////////////////////////////////////////////////////
        void soundFadeIn( const SoundIdentityInterfacePtr & _emitter, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            SoundAffectorCallbackPtr callback = createSoundAffectorCallback( _emitter, _cb, _args );

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            AffectorPtr affector =
                m_affectorCreatorSound.create( ETA_POSITION
                    , easing
                    , callback, [this, _emitter]( float _volume ) { this->___soundFade( _emitter, _volume ); }
                    , 1.f, 0.f, _time
                );

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            affectorable->addAffector( affector );
        }
        //////////////////////////////////////////////////////////////////////////
        SoundIdentityInterfacePtr soundFadeOut( const ConstString & _resourceName, bool _loop, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            SoundIdentityInterfacePtr sourceEmitter = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cb, _args );

            MENGINE_ASSERTION_MEMORY_PANIC( sourceEmitter, nullptr, "can't get resource '%s'"
                , _resourceName.c_str()
            );

            if( SOUND_SERVICE()
                ->playEmitter( sourceEmitter ) == false )
            {
                LOGGER_ERROR( "invalid play '%s'"
                    , _resourceName.c_str()
                );

                return nullptr;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            MENGINE_ASSERTION_MEMORY_PANIC( easing, nullptr, "invalid found easing '%s'"
                , _easingType.c_str()
            );

            AffectorPtr affector =
                m_affectorCreatorSound.create( ETA_POSITION
                    , easing
                    , nullptr, [this, sourceEmitter]( float _value ) { this->___soundFade( sourceEmitter, _value ); }
                    , 0.f, 1.f, _time
                );

            MENGINE_ASSERTION_MEMORY_PANIC( affector, nullptr, "invalid create affector" );

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            affectorable->addAffector( affector );

            return sourceEmitter;
        }
        //////////////////////////////////////////////////////////////////////////
        void soundStop( const SoundIdentityInterfacePtr & _emitter )
        {
            SOUND_SERVICE()
                ->stopEmitter( _emitter );
        }
        //////////////////////////////////////////////////////////////////////////
        void soundPause( const SoundIdentityInterfacePtr & _emitter )
        {
            SOUND_SERVICE()
                ->pauseEmitter( _emitter );
        }
        //////////////////////////////////////////////////////////////////////////
        void soundResume( const SoundIdentityInterfacePtr & _emitter )
        {
            SOUND_SERVICE()
                ->resumeEmitter( _emitter );
        }
        //////////////////////////////////////////////////////////////////////////
        bool isSoundStop( const SoundIdentityInterfacePtr & _emitter )
        {
            return SOUND_SERVICE()
                ->isEmitterStop( _emitter );
        }
        //////////////////////////////////////////////////////////////////////////
        bool isSoundPlay( const SoundIdentityInterfacePtr & _emitter )
        {
            return SOUND_SERVICE()
                ->isEmitterPlay( _emitter );
        }
        //////////////////////////////////////////////////////////////////////////
        bool isSoundPause( const SoundIdentityInterfacePtr & _emitter )
        {
            return SOUND_SERVICE()
                ->isEmitterPause( _emitter );
        }
        //////////////////////////////////////////////////////////////////////////
        void voiceStop( const SoundIdentityInterfacePtr & _emitter )
        {
            SOUND_SERVICE()
                ->stopEmitter( _emitter );
        }
        //////////////////////////////////////////////////////////////////////////
        void soundSourceSetVolume( const SoundIdentityInterfacePtr & _emitter, float _volume )
        {
            if( SOUND_SERVICE()
                ->setSourceVolume( _emitter, _volume, _volume, true ) == false )
            {
                LOGGER_ERROR( "invalid source volume %d"
                    , _emitter->getId()
                );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        float soundSourceGetVolume( const SoundIdentityInterfacePtr & _emitter )
        {
            float volume = SOUND_SERVICE()
                ->getSourceVolume( _emitter );

            return volume;
        }
        //////////////////////////////////////////////////////////////////////////
        void soundSetVolume( float _volume )
        {
            SOUND_SERVICE()
                ->setSoundVolume( STRINGIZE_STRING_LOCAL( "Generic" ), _volume, 0.f );
        }
        //////////////////////////////////////////////////////////////////////////
        float soundGetVolume()
        {
            float volume = SOUND_SERVICE()
                ->getSoundVolume( STRINGIZE_STRING_LOCAL( "Generic" ) );

            return volume;
        }
        //////////////////////////////////////////////////////////////////////////
        void commonSetVolume( float _volume )
        {
            SOUND_SERVICE()
                ->setCommonVolume( STRINGIZE_STRING_LOCAL( "Generic" ), _volume, _volume );
        }
        //////////////////////////////////////////////////////////////////////////
        float commonGetVolume()
        {
            return SOUND_SERVICE()
                ->getCommonVolume( STRINGIZE_STRING_LOCAL( "Generic" ) );
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAmplifierMusicCallback
            : public AmplifierMusicCallbackInterface
            , public Factorable
        {
        public:
            PythonAmplifierMusicCallback( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {
            }

            ~PythonAmplifierMusicCallback() override
            {
            }

        protected:
            void onMusicPause() override
            {
                m_cb.call_args( 0, m_args );
            }

            void onMusicResume() override
            {
                m_cb.call_args( 1, m_args );
            }

            void onMusicStop() override
            {
                m_cb.call_args( 2, m_args );
            }

            void onMusicEnd() override
            {
                m_cb.call_args( 3, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        void musicPlay( const ConstString & _resourceMusic, float _pos, bool _isLooped, const pybind::object & _cb, const pybind::args & _args )
        {
            if( SERVICE_EXIST( AmplifierInterface ) == false )
            {
                return;
            }

            AmplifierMusicCallbackInterfacePtr cb = nullptr;

            if( _cb.is_callable() == true )
            {
                cb = Helper::makeFactorableUnique<PythonAmplifierMusicCallback>( _cb, _args );
            }

            AMPLIFIER_SERVICE()
                ->playMusic( _resourceMusic, _pos, _isLooped, cb );
        }
        //////////////////////////////////////////////////////////////////////////
        void musicSetVolume( float _volume )
        {
            SOUND_SERVICE()
                ->setMusicVolume( STRINGIZE_STRING_LOCAL( "Generic" ), _volume, _volume );
        }
        //////////////////////////////////////////////////////////////////////////
        float musicGetVolume()
        {
            return SOUND_SERVICE()
                ->getMusicVolume( STRINGIZE_STRING_LOCAL( "Generic" ) );
        }
        //////////////////////////////////////////////////////////////////////////
        void musicSetVolumeTag( const ConstString & _tag, float _volume, float _default )
        {
            SOUND_SERVICE()
                ->setMusicVolume( _tag, _volume, _default );
        }
        //////////////////////////////////////////////////////////////////////////
        float musicGetVolumeTag( const ConstString & _tag )
        {
            return SOUND_SERVICE()
                ->getMusicVolume( _tag );
        }
        //////////////////////////////////////////////////////////////////////////
        void voiceSetVolume( float _volume )
        {
            SOUND_SERVICE()
                ->setVoiceVolume( STRINGIZE_STRING_LOCAL( "Generic" ), _volume, _volume );
        }
        //////////////////////////////////////////////////////////////////////////
        float voiceGetVolume()
        {
            return SOUND_SERVICE()
                ->getVoiceVolume( STRINGIZE_STRING_LOCAL( "Generic" ) );
        }
        //////////////////////////////////////////////////////////////////////////
        void musicStop()
        {
            AMPLIFIER_SERVICE()
                ->stopMusic();
        }
        //////////////////////////////////////////////////////////////////////////
        bool musicPause()
        {
            bool successful = AMPLIFIER_SERVICE()
                ->pauseMusic();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool musicResume()
        {
            bool successful = AMPLIFIER_SERVICE()
                ->resumeMusic();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        float musicGetDuration()
        {
            float posMs = AMPLIFIER_SERVICE()
                ->getDuration();

            return posMs;
        }
        //////////////////////////////////////////////////////////////////////////
        float musicGetPosMs()
        {
            float posMs = AMPLIFIER_SERVICE()
                ->getPosMs();

            return posMs;
        }
        //////////////////////////////////////////////////////////////////////////
        void musicSetPosMs( float _posMs )
        {
            AMPLIFIER_SERVICE()
                ->setPosMs( _posMs );
        }
        //////////////////////////////////////////////////////////////////////////
        void ___musicFade( float _volume )
        {
            SOUND_SERVICE()
                ->setMusicVolume( STRINGIZE_STRING_LOCAL( "Fade" ), _volume, _volume );
        }
        //////////////////////////////////////////////////////////////////////////
        class MusicAffectorCallback
            : public AffectorCallbackInterface
        {
        public:
            MusicAffectorCallback()
            {
            }

            ~MusicAffectorCallback() override
            {
            }

        public:
            void initialize( const pybind::object & _cb, const pybind::args & _args )
            {
                m_cb = _cb;
                m_args = _args;
            }

        protected:
            void onAffectorEnd( uint32_t _id, bool _isEnd ) override
            {
                if( m_cb.is_invalid() == true )
                {
                    return;
                }

                if( m_cb.is_none() == true )
                {
                    return;
                }

                AMPLIFIER_SERVICE()
                    ->stopMusic();

                m_cb.call_args( _id, _isEnd, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<MusicAffectorCallback> MusicAffectorCallbackPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryMusicAffectorCallback;
        //////////////////////////////////////////////////////////////////////////
        MusicAffectorCallbackPtr createMusicAffectorCallback( const pybind::object & _cb, const pybind::args & _args )
        {
            MusicAffectorCallbackPtr callback = m_factoryMusicAffectorCallback->createObject( MENGINE_DOCUMENT_PYBIND );
            callback->initialize( _cb, _args );

            return callback;
        }
        //////////////////////////////////////////////////////////////////////////
        AFFECTOR_ID m_affectorMusicID;
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float> m_affectorCreatorMusic;
        //////////////////////////////////////////////////////////////////////////
        uint32_t musicFadeIn( float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( SERVICE_EXIST( AmplifierInterface ) == false )
            {
                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            MusicAffectorCallbackPtr callback = createMusicAffectorCallback( _cb, _args );

            AffectorPtr affector =
                m_affectorCreatorMusic.create( ETA_POSITION
                    , easing
                    , callback
                    , [this]( float _value ) { this->___musicFade( _value ); }
                    , 1.f, 0.f, _time
                );

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            if( m_affectorMusicID != 0 )
            {
                if( affectorable->hasAffector( m_affectorMusicID ) == true )
                {
                    affectorable->stopAffector( m_affectorMusicID );
                }
            }

            AFFECTOR_ID id = affectorable->addAffector( affector );

            m_affectorMusicID = id;

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t musicFadeOut( const ConstString & _resourceMusic, float _pos, bool _isLooped, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
        {
            if( SERVICE_EXIST( AmplifierInterface ) == false )
            {
                return 0;
            }

            AmplifierMusicCallbackInterfacePtr cb = nullptr;

            if( _cb.is_callable() == true )
            {
                cb = Helper::makeFactorableUnique<PythonAmplifierMusicCallback>( _cb, _args );
            }

            if( AMPLIFIER_SERVICE()
                ->playMusic( _resourceMusic, _pos, _isLooped, cb ) == false )
            {
                LOGGER_ERROR( "invalid play music '%s' "
                    , _resourceMusic.c_str()
                );

                return 0;
            }

            EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

            AffectorPtr affector =
                m_affectorCreatorMusic.create( ETA_POSITION
                    , easing
                    , nullptr
                    , [this]( float _value ) { this->___musicFade( _value ); }
                    , 0.f, 1.f, _time
                );

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            if( m_affectorMusicID != 0 )
            {
                if( affectorable->hasAffector( m_affectorMusicID ) == true )
                {
                    affectorable->stopAffector( m_affectorMusicID );
                }
            }

            AFFECTOR_ID id = affectorable->addAffector( affector );

            m_affectorMusicID = id;

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void soundMute( bool _mute )
        {
            SOUND_SERVICE()
                ->mute( _mute );
        }
        //////////////////////////////////////////////////////////////////////////
        bool isMute()
        {
            bool mute = SOUND_SERVICE()
                ->isMute();

            return mute;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isSilent()
        {
            bool silent = SOUND_SYSTEM()
                ->isSilent();

            return silent;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    SoundScriptEmbedding::SoundScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundScriptEmbedding::~SoundScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        SoundScriptMethod * soundScriptMethod = new FactorableUnique<SoundScriptMethod>();

        pybind::interface_<SoundIdentityInterface, pybind::bases<Mixin> >( _kernel, "SoundIdentity" )
            .def( "getId", &SoundIdentityInterface::getId )
            .def( "isStreamable", &SoundIdentityInterface::isStreamable )
            .def( "getLoop", &SoundIdentityInterface::getLoop )
            ;

        pybind::def_functor( _kernel, "hasSound", soundScriptMethod, &SoundScriptMethod::hasSound );
        pybind::def_functor( _kernel, "isMute", soundScriptMethod, &SoundScriptMethod::isMute );
        pybind::def_functor( _kernel, "isSilent", soundScriptMethod, &SoundScriptMethod::isSilent );
        pybind::def_functor( _kernel, "soundMute", soundScriptMethod, &SoundScriptMethod::soundMute );

        pybind::def_functor( _kernel, "commonSetVolume", soundScriptMethod, &SoundScriptMethod::commonSetVolume );
        pybind::def_functor( _kernel, "commonGetVolume", soundScriptMethod, &SoundScriptMethod::commonGetVolume );

        pybind::def_functor_args( _kernel, "soundPlay", soundScriptMethod, &SoundScriptMethod::soundPlay );
        pybind::def_functor_args( _kernel, "soundPlayFromPosition", soundScriptMethod, &SoundScriptMethod::soundPlayFromPosition );
        pybind::def_functor( _kernel, "soundStop", soundScriptMethod, &SoundScriptMethod::soundStop );
        pybind::def_functor( _kernel, "soundPause", soundScriptMethod, &SoundScriptMethod::soundPause );
        pybind::def_functor( _kernel, "soundResume", soundScriptMethod, &SoundScriptMethod::soundResume );
        pybind::def_functor( _kernel, "isSoundStop", soundScriptMethod, &SoundScriptMethod::isSoundStop );
        pybind::def_functor( _kernel, "isSoundPlay", soundScriptMethod, &SoundScriptMethod::isSoundPlay );
        pybind::def_functor( _kernel, "isSoundPause", soundScriptMethod, &SoundScriptMethod::isSoundPause );
        pybind::def_functor( _kernel, "soundSourceSetVolume", soundScriptMethod, &SoundScriptMethod::soundSourceSetVolume );
        pybind::def_functor( _kernel, "soundSourceGetVolume", soundScriptMethod, &SoundScriptMethod::soundSourceGetVolume );
        pybind::def_functor( _kernel, "soundSetVolume", soundScriptMethod, &SoundScriptMethod::soundSetVolume );
        pybind::def_functor( _kernel, "soundGetVolume", soundScriptMethod, &SoundScriptMethod::soundGetVolume );
        pybind::def_functor( _kernel, "soundGetPosition", soundScriptMethod, &SoundScriptMethod::soundGetPosMs );
        pybind::def_functor( _kernel, "soundSetPosition", soundScriptMethod, &SoundScriptMethod::soundSetPosMs );
        pybind::def_functor_args( _kernel, "soundFadeIn", soundScriptMethod, &SoundScriptMethod::soundFadeIn );
        pybind::def_functor_args( _kernel, "soundFadeOut", soundScriptMethod, &SoundScriptMethod::soundFadeOut );

        pybind::def_functor_args( _kernel, "musicPlay", soundScriptMethod, &SoundScriptMethod::musicPlay );
        pybind::def_functor( _kernel, "musicSetVolume", soundScriptMethod, &SoundScriptMethod::musicSetVolume );
        pybind::def_functor( _kernel, "musicGetVolume", soundScriptMethod, &SoundScriptMethod::musicGetVolume );
        pybind::def_functor( _kernel, "musicSetVolumeTag", soundScriptMethod, &SoundScriptMethod::musicSetVolumeTag );
        pybind::def_functor( _kernel, "musicGetVolumeTag", soundScriptMethod, &SoundScriptMethod::musicGetVolumeTag );
        pybind::def_functor( _kernel, "musicStop", soundScriptMethod, &SoundScriptMethod::musicStop );
        pybind::def_functor( _kernel, "musicPause", soundScriptMethod, &SoundScriptMethod::musicPause );
        pybind::def_functor( _kernel, "musicResume", soundScriptMethod, &SoundScriptMethod::musicResume );
        pybind::def_functor( _kernel, "musicGetDuration", soundScriptMethod, &SoundScriptMethod::musicGetDuration );
        pybind::def_functor( _kernel, "musicGetLengthMs", soundScriptMethod, &SoundScriptMethod::musicGetDuration );
        pybind::def_functor( _kernel, "musicGetPosMs", soundScriptMethod, &SoundScriptMethod::musicGetPosMs );
        pybind::def_functor( _kernel, "musicSetPosMs", soundScriptMethod, &SoundScriptMethod::musicSetPosMs );
        pybind::def_functor_args( _kernel, "musicFadeIn", soundScriptMethod, &SoundScriptMethod::musicFadeIn );
        pybind::def_functor_args( _kernel, "musicFadeOut", soundScriptMethod, &SoundScriptMethod::musicFadeOut );


        pybind::def_functor_args( _kernel, "voicePlay", soundScriptMethod, &SoundScriptMethod::voicePlay );
        pybind::def_functor( _kernel, "voiceStop", soundScriptMethod, &SoundScriptMethod::voiceStop );
        pybind::def_functor( _kernel, "voicePause", soundScriptMethod, &SoundScriptMethod::voicePause );
        pybind::def_functor( _kernel, "voiceResume", soundScriptMethod, &SoundScriptMethod::voiceResume );
        pybind::def_functor( _kernel, "voiceSetVolume", soundScriptMethod, &SoundScriptMethod::voiceSetVolume );
        pybind::def_functor( _kernel, "voiceGetVolume", soundScriptMethod, &SoundScriptMethod::voiceGetVolume );

        m_implement = soundScriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SoundScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        m_implement = nullptr;
    }
}

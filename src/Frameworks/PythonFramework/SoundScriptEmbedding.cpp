#include "SoundScriptEmbedding.h"

#include "Interface/SoundServiceInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/DocumentInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Logger.h"
#include "Kernel/Affectorable.h"
#include "Kernel/AffectorHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionResourceType.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ResourceHelper.h"
#include "Kernel/ResourceSound.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        class SoundScriptMethod
            : public Factorable
        {
        public:
            SoundScriptMethod()
            {
            }

            ~SoundScriptMethod() override
            {
            }

        public:
            bool initialize()
            {
                m_factorySoundAffectorCallback = Helper::makeFactoryPool<SoundAffectorCallback, 4>( MENGINE_DOCUMENT_FACTORABLE );

                m_affectorCreatorSound = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float>>( MENGINE_DOCUMENT_FACTORABLE );
                m_affectorCreatorSound->initialize();

                return true;
            }

            void finalize()
            {
                m_affectorCreatorSound->finalize();
                m_affectorCreatorSound = nullptr;

                MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundAffectorCallback );

                m_factorySoundAffectorCallback = nullptr;
            }

        public:
            //////////////////////////////////////////////////////////////////////////
            class MySoundNodeListener
                : public Factorable
                , public SoundListenerInterface
            {
                DECLARE_FACTORABLE( MySoundNodeListener );

            public:
                MySoundNodeListener( const ResourceSoundPtr & _resource, const pybind::object & _cb, const pybind::args & _args )
                    : m_resource( _resource )
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
                    if( SOUND_SERVICE()
                        ->releaseSoundSource( _emitter ) == false )
                    {
                        uint32_t id = _emitter->getId();

                        LOGGER_ERROR( "resource sound '%s' emitter invalid release sound '%u'"
                            , m_resource->getName().c_str()
                            , id
                        );
                    }

                    if( m_cb.is_callable() == true )
                    {
                        m_cb.call_args( _emitter, 0, m_args );
                    }

                    m_resource->release();
                    m_resource = nullptr;
                }

                void onSoundEnd( const SoundIdentityInterfacePtr & _emitter ) override
                {
                    if( SOUND_SERVICE()
                        ->releaseSoundSource( _emitter ) == false )
                    {
                        uint32_t id = _emitter->getId();

                        LOGGER_ERROR( "resource sound '%s' emitter invalid release sound '%u'"
                            , m_resource->getName().c_str()
                            , id
                        );
                    }

                    if( m_cb.is_callable() == true )
                    {
                        m_cb.call_args( _emitter, 1, m_args );
                    }

                    m_resource->release();
                    m_resource = nullptr;
                }

            protected:
                ResourceSoundPtr m_resource;
                pybind::object m_cb;
                pybind::args m_args;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<MySoundNodeListener> MySoundNodeListenerPtr;
            //////////////////////////////////////////////////////////////////////////
            bool hasSound( const ConstString & _resourceName )
            {
                ResourcePtr resource;
                if( RESOURCE_SERVICE()
                    ->hasResource( ConstString::none(), _resourceName, false, &resource ) == false )
                {
                    return false;
                }

                const ConstString & resourceType = resource->getType();

                if( resourceType != STRINGIZE_STRING_LOCAL( "ResourceSound" ) )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factorySoundNodeListener;
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr s_createSoundSource( const ConstString & _resourceName, bool _loop, ESoundSourceCategory _category, const pybind::object & _cb, const pybind::args & _args, const DocumentInterfacePtr & _doc )
            {
                MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( _resourceName, ResourceSoundPtr, nullptr, "resource '%s' type does not match 'ResourceSound'"
                    , _resourceName.c_str()
                );

                ResourceSoundPtr resource = RESOURCE_SERVICE()
                    ->getResourceReference( ConstString::none(), _resourceName );

                MENGINE_ASSERTION_MEMORY_PANIC( resource );

                if( resource->compile() == false )
                {
                    LOGGER_ERROR( "resource sound '%s' type '%s' is not compile!"
                        , resource->getName().c_str()
                        , resource->getType().c_str()
                    );

                    return nullptr;
                }

                SoundBufferInterfacePtr soundBuffer = resource->createSoundBuffer( _doc );

                if( soundBuffer == nullptr )
                {
                    resource->release();

                    return nullptr;
                }

                bool streamable = resource->isStreamable();

                SoundIdentityInterfacePtr soundIdentity = SOUND_SERVICE()
                    ->createSoundIdentity( true, soundBuffer, _category, streamable
                        , _doc
                    );

                if( soundIdentity == nullptr )
                {
                    LOGGER_ERROR( "resource sound '%s' invalid create identity"
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
                    ->setLoop( soundIdentity, _loop );

                float volume = resource->getDefaultVolume();

                if( SOUND_SERVICE()
                    ->setSourceVolume( soundIdentity, volume, volume, true ) == false )
                {
                    LOGGER_ERROR( "resource sound '%s' invalid set volume %f"
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

                MySoundNodeListenerPtr snlistener = Helper::makeFactorableUnique<MySoundNodeListener>( _doc, resource, _cb, _args );

                soundIdentity->setSoundListener( snlistener );

                return soundIdentity;
            }
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr soundPlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb, const pybind::args & _args )
            {
                SoundIdentityInterfacePtr soundIdentity = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cb, _args, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( soundIdentity, "can't get resource '%s'"
                    , _resourceName.c_str()
                );

                LOGGER_INFO( "sound", "[script] sound play resource '%s' file '%s'"
                    , _resourceName.c_str()
                    , Helper::getResourceFilePathByName( _resourceName ).c_str()
                );

                if( SOUND_SERVICE()
                    ->playEmitter( soundIdentity ) == false )
                {
                    LOGGER_ERROR( "invalid play '%s'"
                        , _resourceName.c_str()
                    );

                    return nullptr;
                }

                return soundIdentity;
            }
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr voicePlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb, const pybind::args & _args )
            {
                SoundIdentityInterfacePtr soundIdentity = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_VOICE, _cb, _args, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( soundIdentity, "can't get resource '%s'"
                    , _resourceName.c_str()
                );

                LOGGER_INFO( "sound", "[script] voice play resource '%s' file '%s'"
                    , _resourceName.c_str()
                    , Helper::getResourceFilePathByName( _resourceName ).c_str()
                );

                if( SOUND_SERVICE()
                    ->playEmitter( soundIdentity ) == false )
                {
                    LOGGER_ERROR( "invalid play '%s'"
                        , _resourceName.c_str()
                    );

                    return nullptr;
                }

                return soundIdentity;
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
                SoundIdentityInterfacePtr sourceEmitter = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cb, _args, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( sourceEmitter, "can't get resource '%s'"
                    , _resourceName.c_str()
                );

                if( SOUND_SERVICE()
                    ->setPosMs( sourceEmitter, _position ) == false )
                {
                    LOGGER_ERROR( "resource sound '%s' invalid set pos '%f'"
                        , _resourceName.c_str()
                        , _position
                    );

                    return nullptr;
                }

                LOGGER_INFO( "sound", "[script] voice play resource '%s' file '%s' from position '%f'"
                    , _resourceName.c_str()
                    , Helper::getResourceFilePathByName( _resourceName ).c_str()
                    , _position
                );


                if( SOUND_SERVICE()
                    ->playEmitter( sourceEmitter ) == false )
                {
                    LOGGER_ERROR( "resource sound '%s' invalid play '%f'"
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
                DECLARE_FACTORABLE( SoundAffectorCallback );

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
                    m_soundIdentity = _emitter;
                    m_cb = _cb;
                    m_args = _args;
                }

            protected:
                void onAffectorEnd( uint32_t _id, bool _isEnd ) override
                {
                    if( _isEnd == true )
                    {
                        SOUND_SERVICE()
                            ->stopEmitter( m_soundIdentity );
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
                SoundIdentityInterfacePtr m_soundIdentity;
                pybind::object m_cb;
                pybind::args m_args;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<SoundAffectorCallback> SoundAffectorCallbackPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factorySoundAffectorCallback;
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
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float>> m_affectorCreatorSound;
            //////////////////////////////////////////////////////////////////////////
            void soundFadeIn( const SoundIdentityInterfacePtr & _emitter, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                SoundAffectorCallbackPtr callback = createSoundAffectorCallback( _emitter, _cb, _args );

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                AffectorPtr affector = m_affectorCreatorSound->create( ETA_POSITION
                    , easing
                    , callback, [this, _emitter]( float _volume )
                {
                    this->___soundFade( _emitter, _volume );
                }
                    , 1.f, 0.f, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->addAffector( affector );
            }
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr soundFadeOut( const ConstString & _resourceName, bool _loop, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                SoundIdentityInterfacePtr soundIdentity = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cb, _args, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( soundIdentity, "can't get resource '%s'"
                    , _resourceName.c_str()
                );

                LOGGER_INFO( "sound", "[script] sound fade out resource '%s' file '%s'"
                    , _resourceName.c_str()
                    , Helper::getResourceFilePathByName( _resourceName ).c_str()
                );

                if( SOUND_SERVICE()
                    ->playEmitter( soundIdentity ) == false )
                {
                    LOGGER_ERROR( "resource sound '%s' invalid play"
                        , _resourceName.c_str()
                    );

                    return nullptr;
                }

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                MENGINE_ASSERTION_MEMORY_PANIC( easing, "invalid found easing '%s'"
                    , _easingType.c_str()
                );

                AffectorPtr affector = m_affectorCreatorSound->create( ETA_POSITION
                    , easing
                    , nullptr, [this, soundIdentity]( float _value )
                {
                    this->___soundFade( soundIdentity, _value );
                }
                    , 0.f, 1.f, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->addAffector( affector );

                return soundIdentity;
            }
            //////////////////////////////////////////////////////////////////////////
            void soundFadeInTo( const SoundIdentityInterfacePtr & _emitter, float _to, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                SoundAffectorCallbackPtr callback = createSoundAffectorCallback( _emitter, _cb, _args );

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                float volume = SOUND_SERVICE()
                    ->getSourceMixerVolume( _emitter, STRINGIZE_STRING_LOCAL( "Fade" ) );

                AffectorPtr affector = m_affectorCreatorSound->create( ETA_POSITION
                    , easing
                    , callback, [this, _emitter]( float _volume )
                {
                    this->___soundFade( _emitter, _volume );
                }
                    , volume, _to, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->addAffector( affector );
            }
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr soundFadeOutTo( const ConstString & _resourceName, bool _loop, float _to, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                SoundIdentityInterfacePtr soundIdentity = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cb, _args, MENGINE_DOCUMENT_PYBIND );

                MENGINE_ASSERTION_MEMORY_PANIC( soundIdentity, "can't get resource '%s'"
                    , _resourceName.c_str()
                );

                LOGGER_INFO( "sound", "[script] sound fade out to resource '%s' file '%s'"
                    , _resourceName.c_str()
                    , Helper::getResourceFilePathByName( _resourceName ).c_str()
                );

                if( SOUND_SERVICE()
                    ->playEmitter( soundIdentity ) == false )
                {
                    LOGGER_ERROR( "resource sound '%s' invalid play"
                        , _resourceName.c_str()
                    );

                    return nullptr;
                }

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                MENGINE_ASSERTION_MEMORY_PANIC( easing, "invalid found easing '%s'"
                    , _easingType.c_str()
                );

                float volume = SOUND_SERVICE()
                    ->getSourceMixerVolume( soundIdentity, STRINGIZE_STRING_LOCAL( "Fade" ) );

                AffectorPtr affector = m_affectorCreatorSound->create( ETA_POSITION
                    , easing
                    , nullptr, [this, soundIdentity]( float _value )
                {
                    this->___soundFade( soundIdentity, _value );
                }
                    , volume, _to, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->addAffector( affector );

                return soundIdentity;
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
                    LOGGER_ERROR( "sound emitter [%u] invalid set source volume [%f]"
                        , _emitter->getId()
                        , _volume
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
        typedef IntrusivePtr< SoundScriptMethod> SoundScriptMethodPtr;
        //////////////////////////////////////////////////////////////////////////
    }
    SoundScriptEmbedding::SoundScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundScriptEmbedding::~SoundScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SoundScriptMethodPtr soundScriptMethod = Helper::makeFactorableUnique<SoundScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( soundScriptMethod->initialize() == false )
        {
            return false;
        }

        pybind::interface_<SoundIdentityInterface, pybind::bases<Mixin>>( _kernel, "SoundIdentity" )
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
        pybind::def_functor_args( _kernel, "soundFadeInTo", soundScriptMethod, &SoundScriptMethod::soundFadeInTo );
        pybind::def_functor_args( _kernel, "soundFadeOutTo", soundScriptMethod, &SoundScriptMethod::soundFadeOutTo );

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
    void SoundScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        SoundScriptMethodPtr soundScriptMethod = m_implement;
        soundScriptMethod->finalize();

        m_implement = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}

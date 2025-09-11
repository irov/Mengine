#include "SoundScriptEmbedding.h"

#include "Interface/SoundServiceInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/DocumentInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"
#include "Environment/Python/PythonCallbackProvider.h"

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
#include "Kernel/VocabularyHelper.h"

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
                m_factorySoundAffectorCallback = Helper::makeFactoryPool<PythonSoundAffectorCallback, 4>( MENGINE_DOCUMENT_FACTORABLE );

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
            class PythonSoundListener
                : public SoundListenerInterface
                , public PythonCallbackProvider
                , public Factorable
            {
                DECLARE_FACTORABLE( PythonSoundListener );

            public:
                PythonSoundListener( const ResourceSoundPtr & _resource, const pybind::object & _cbs, const pybind::args & _args )
                    : PythonCallbackProvider( _cbs, _args )
                    , m_resource( _resource )
                {
                }

                ~PythonSoundListener() override
                {
                    if( m_resource != nullptr )
                    {
                        m_resource->release();
                        m_resource = nullptr;
                    }
                }

            protected:
                void onSoundPlay( const SoundIdentityInterfacePtr & _identity ) override
                {
                    this->call_method( "onSoundPlay", _identity );
                }

                void onSoundPause( const SoundIdentityInterfacePtr & _identity ) override
                {
                    this->call_method( "onSoundPause", _identity );
                }

                void onSoundResume( const SoundIdentityInterfacePtr & _identity ) override
                {
                    this->call_method( "onSoundResume", _identity );
                }

                void onSoundStop( const SoundIdentityInterfacePtr & _identity ) override
                {
                    if( SOUND_SERVICE()
                        ->releaseSoundIdentity( _identity ) == false )
                    {
                        LOGGER_ERROR( "resource sound '%s' emitter invalid release sound '%u'"
                            , m_resource->getName().c_str()
                            , _identity->getUniqueIdentity()
                        );
                    }

                    this->call_method( "onSoundStop", _identity );

                    m_resource->release();
                    m_resource = nullptr;
                }

                void onSoundEnd( const SoundIdentityInterfacePtr & _identity ) override
                {
                    if( SOUND_SERVICE()
                        ->releaseSoundIdentity( _identity ) == false )
                    {
                        LOGGER_ERROR( "resource sound '%s' emitter invalid release sound '%u'"
                            , m_resource->getName().c_str()
                            , _identity->getUniqueIdentity()
                        );
                    }

                    this->call_method( "onSoundEnd", _identity );

                    m_resource->release();
                    m_resource = nullptr;
                }

            protected:
                ResourceSoundPtr m_resource;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PythonSoundListener, SoundListenerInterface> PythonSoundListenerPtr;
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

                if( resourceType != ResourceSound::getFactorableType() )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factorySoundNodeListener;
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr s_createSoundIdentity( const ConstString & _resourceName, bool _loop, ESoundSourceCategory _category, const pybind::object & _cbs, const pybind::args & _args, const DocumentInterfacePtr & _doc )
            {
                MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( _resourceName, ResourceSoundPtr, nullptr, "resource '%s' type does not match 'ResourceSound'"
                    , _resourceName.c_str()
                );

                ResourceSoundPtr resource = RESOURCE_SERVICE()
                    ->getResourceReference( ConstString::none(), _resourceName );

                MENGINE_ASSERTION_MEMORY_PANIC( resource, "can't get resource '%s'"
                    , _resourceName.c_str()
                );

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
                    LOGGER_ERROR( "resource sound '%s' invalid create sound buffer"
                        , _resourceName.c_str()
                    );

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

                    resource->release();

                    return nullptr;
                }

                SOUND_SERVICE()
                    ->setLoop( soundIdentity, _loop );

                float volume = resource->getDefaultVolume();

                if( SOUND_SERVICE()
                    ->setSourceVolume( soundIdentity, volume, volume, MENGINE_MIXER_VALUE_DEFAULT_SPEED, true ) == false )
                {
                    LOGGER_ERROR( "resource sound '%s' invalid set volume %f"
                        , _resourceName.c_str()
                        , volume
                    );

                    resource->release();

                    return nullptr;
                }

                PythonSoundListenerPtr snlistener = Helper::makeFactorableUnique<PythonSoundListener>( _doc, resource, _cbs, _args );

                soundIdentity->setSoundListener( snlistener );

                return soundIdentity;
            }
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr soundPlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cbs, const pybind::args & _args )
            {
                LOGGER_INFO( "sound", "[script] sound play resource '%s' file '%s'"
                    , _resourceName.c_str()
                    , Helper::getResourceFilePathByName( _resourceName ).c_str()
                );

                SoundIdentityInterfacePtr soundIdentity = s_createSoundIdentity( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cbs, _args, MENGINE_DOCUMENT_PYTHON );

                if( soundIdentity == nullptr )
                {
                    LOGGER_ERROR( "invalid create sound source '%s'"
                        , _resourceName.c_str()
                    );

                    return nullptr;
                }

                if( SOUND_SERVICE()
                    ->playEmitter( soundIdentity ) == false )
                {
                    LOGGER_WARNING( "invalid sound play resource '%s' file '%s'"
                        , _resourceName.c_str()
                        , Helper::getResourceFilePathByName( _resourceName ).c_str()
                    );

                    SOUND_SERVICE()
                        ->releaseSoundIdentity( soundIdentity );

                    return nullptr;
                }

                return soundIdentity;
            }
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr voicePlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cbs, const pybind::args & _args )
            {
                LOGGER_INFO( "sound", "[script] voice play resource '%s' file '%s'"
                    , _resourceName.c_str()
                    , Helper::getResourceFilePathByName( _resourceName ).c_str()
                );

                SoundIdentityInterfacePtr soundIdentity = s_createSoundIdentity( _resourceName, _loop, ES_SOURCE_CATEGORY_VOICE, _cbs, _args, MENGINE_DOCUMENT_PYTHON );

                if( soundIdentity == nullptr )
                {
                    LOGGER_ERROR( "invalid create voice source '%s'"
                        , _resourceName.c_str()
                    );

                    return nullptr;
                }

                if( SOUND_SERVICE()
                    ->playEmitter( soundIdentity ) == false )
                {
                    LOGGER_WARNING( "invalid voice play resource '%s' file '%s' play"
                        , _resourceName.c_str()
                        , Helper::getResourceFilePathByName( _resourceName ).c_str()
                    );

                    SOUND_SERVICE()
                        ->releaseSoundIdentity( soundIdentity );

                    return nullptr;
                }

                return soundIdentity;
            }
            //////////////////////////////////////////////////////////////////////////
            bool voicePause( const SoundIdentityInterfacePtr & _identity )
            {
                bool successful = SOUND_SERVICE()
                    ->pauseEmitter( _identity );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            bool voiceResume( const SoundIdentityInterfacePtr & _identity )
            {
                bool successful = SOUND_SERVICE()
                    ->resumeEmitter( _identity );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr soundPlayFromPosition( const ConstString & _resourceName, float _position, bool _loop, const pybind::object & _cbs, const pybind::args & _args )
            {
                LOGGER_INFO( "sound", "[script] voice play resource '%s' file '%s' from position '%f'"
                    , _resourceName.c_str()
                    , Helper::getResourceFilePathByName( _resourceName ).c_str()
                    , _position
                );

                SoundIdentityInterfacePtr soundIdentity = s_createSoundIdentity( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cbs, _args, MENGINE_DOCUMENT_PYTHON );

                if( soundIdentity == nullptr )
                {
                    LOGGER_ERROR( "invalid create sound source '%s'"
                        , _resourceName.c_str()
                    );

                    return nullptr;
                }

                if( SOUND_SERVICE()
                    ->setPosition( soundIdentity, _position ) == false )
                {
                    LOGGER_ERROR( "resource sound '%s' invalid set pos '%f'"
                        , _resourceName.c_str()
                        , _position
                    );

                    SOUND_SERVICE()
                        ->releaseSoundIdentity( soundIdentity );

                    return nullptr;
                }

                if( SOUND_SERVICE()
                    ->playEmitter( soundIdentity ) == false )
                {
                    LOGGER_WARNING( "invalid voice play resource '%s' file '%s' from position '%f'"
                        , _resourceName.c_str()
                        , Helper::getResourceFilePathByName( _resourceName ).c_str()
                        , _position
                    );

                    SOUND_SERVICE()
                        ->releaseSoundIdentity( soundIdentity );

                    return nullptr;
                }

                return soundIdentity;
            }
            //////////////////////////////////////////////////////////////////////////
            float soundGetPosition( const SoundIdentityInterfacePtr & _identity )
            {
                float position = SOUND_SERVICE()
                    ->getPosition( _identity );

                return position;
            }
            //////////////////////////////////////////////////////////////////////////
            void soundSetPosition( const SoundIdentityInterfacePtr & _identity, float _position )
            {
                SOUND_SERVICE()
                    ->setPosition( _identity, _position );
            }
            //////////////////////////////////////////////////////////////////////////
            class PythonSoundAffectorCallback
                : public AffectorCallbackInterface
                , public PythonCallbackProvider
                , public Factorable
            {
                DECLARE_FACTORABLE( PythonSoundAffectorCallback );

            public:
                PythonSoundAffectorCallback()
                {
                }

                ~PythonSoundAffectorCallback() override
                {
                }

            public:
                void initialize( const SoundIdentityInterfacePtr & _identity, const pybind::object & _cb, const pybind::args & _args )
                {
                    m_soundIdentity = _identity;

                    PythonCallbackProvider::initialize( _cb, _args );
                }

            protected:
                void onAffectorEnd( bool _isEnd ) override
                {
                    if( _isEnd == true )
                    {
                        SOUND_SERVICE()
                            ->stopEmitter( m_soundIdentity );
                    }

                    this->call_cb( _isEnd );
                }

            protected:
                SoundIdentityInterfacePtr m_soundIdentity;
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<PythonSoundAffectorCallback, AffectorCallbackInterface> PythonSoundAffectorCallbackPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factorySoundAffectorCallback;
            //////////////////////////////////////////////////////////////////////////
            PythonSoundAffectorCallbackPtr createSoundAffectorCallback( const SoundIdentityInterfacePtr & _identity, const pybind::object & _cb, const pybind::args & _args )
            {
                if( _cb.is_none() == true )
                {
                    return nullptr;
                }

                MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid identity create sound affector callback" );

                PythonSoundAffectorCallbackPtr callback = m_factorySoundAffectorCallback->createObject( MENGINE_DOCUMENT_PYTHON );

                callback->initialize( _identity, _cb, _args );

                return callback;
            }
            //////////////////////////////////////////////////////////////////////////
            void ___soundFade( const SoundIdentityInterfacePtr & _identity, float _volume )
            {
                SOUND_SERVICE()
                    ->setSourceMixerVolume( _identity, STRINGIZE_STRING_LOCAL( "Fade" ), _volume, _volume );
            }
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float>> m_affectorCreatorSound;
            //////////////////////////////////////////////////////////////////////////
            bool soundFadeIn( const SoundIdentityInterfacePtr & _identity, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                if( _identity == nullptr )
                {
                    LOGGER_ERROR( "soundFadeIn invalid emitter" );

                    return false;
                }

                PythonSoundAffectorCallbackPtr callback = createSoundAffectorCallback( _identity, _cb, _args );

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                AffectorPtr affector = m_affectorCreatorSound->create( EAFFECTORTYPE_VOLUME
                    , easing
                    , callback, [this, _identity]( float _volume )
                {
                    this->___soundFade( _identity, _volume );
                }
                    , 1.f, 0.f, _time
                    , MENGINE_DOCUMENT_PYTHON
                    );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->addAffector( affector );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr soundFadeOut( const ConstString & _resourceName, bool _loop, float _time, const ConstString & _easingType, const pybind::object & _cbs, const pybind::args & _args )
            {
                LOGGER_INFO( "sound", "[script] sound fade out resource '%s' file '%s'"
                    , _resourceName.c_str()
                    , Helper::getResourceFilePathByName( _resourceName ).c_str()
                );

                SoundIdentityInterfacePtr soundIdentity = s_createSoundIdentity( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cbs, _args, MENGINE_DOCUMENT_PYTHON );

                if( soundIdentity == nullptr )
                {
                    LOGGER_ERROR( "invalid create sound source '%s'"
                        , _resourceName.c_str()
                    );

                    return nullptr;
                }

                if( SOUND_SERVICE()
                    ->playEmitter( soundIdentity ) == false )
                {
                    LOGGER_WARNING( "resource sound '%s' invalid play"
                        , _resourceName.c_str()
                    );

                    SOUND_SERVICE()
                        ->releaseSoundIdentity( soundIdentity );

                    return nullptr;
                }

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                MENGINE_ASSERTION_MEMORY_PANIC( easing, "invalid found easing '%s'"
                    , _easingType.c_str()
                );

                AffectorPtr affector = m_affectorCreatorSound->create( EAFFECTORTYPE_VOLUME
                    , easing
                    , nullptr, [this, soundIdentity]( float _volume )
                {
                    this->___soundFade( soundIdentity, _volume );
                }
                    , 0.f, 1.f, _time
                    , MENGINE_DOCUMENT_PYTHON
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->addAffector( affector );

                return soundIdentity;
            }
            //////////////////////////////////////////////////////////////////////////
            bool soundFadeInTo( const SoundIdentityInterfacePtr & _identity, float _to, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                if( _identity == nullptr )
                {
                    LOGGER_ERROR( "soundFadeInTo invalid emitter" );

                    return false;
                }

                PythonSoundAffectorCallbackPtr callback = createSoundAffectorCallback( _identity, _cb, _args );

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                float volume = SOUND_SERVICE()
                    ->getSourceMixerVolume( _identity, STRINGIZE_STRING_LOCAL( "Fade" ) );

                AffectorPtr affector = m_affectorCreatorSound->create( EAFFECTORTYPE_VOLUME
                    , easing
                    , callback, [this, _identity]( float _volume )
                {
                    this->___soundFade( _identity, _volume );
                }
                    , volume, _to, _time
                    , MENGINE_DOCUMENT_PYTHON
                    );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->addAffector( affector );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            SoundIdentityInterfacePtr soundFadeOutTo( const ConstString & _resourceName, bool _loop, float _to, float _time, const ConstString & _easingType, const pybind::object & _cbs, const pybind::args & _args )
            {
                LOGGER_INFO( "sound", "[script] sound fade out to resource '%s' file '%s'"
                    , _resourceName.c_str()
                    , Helper::getResourceFilePathByName( _resourceName ).c_str()
                );

                SoundIdentityInterfacePtr soundIdentity = s_createSoundIdentity( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cbs, _args, MENGINE_DOCUMENT_PYTHON );

                if( soundIdentity == nullptr )
                {
                    LOGGER_ERROR( "invalid create sound source '%s'"
                        , _resourceName.c_str()
                    );

                    return nullptr;
                }

                if( SOUND_SERVICE()
                    ->playEmitter( soundIdentity ) == false )
                {
                    LOGGER_WARNING( "resource sound '%s' invalid play"
                        , _resourceName.c_str()
                    );

                    SOUND_SERVICE()
                        ->releaseSoundIdentity( soundIdentity );

                    return nullptr;
                }

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                MENGINE_ASSERTION_MEMORY_PANIC( easing, "invalid found easing '%s'"
                    , _easingType.c_str()
                );

                float volume = SOUND_SERVICE()
                    ->getSourceMixerVolume( soundIdentity, STRINGIZE_STRING_LOCAL( "Fade" ) );

                AffectorPtr affector = m_affectorCreatorSound->create( EAFFECTORTYPE_VOLUME
                    , easing
                    , nullptr, [this, soundIdentity]( float _value )
                {
                    this->___soundFade( soundIdentity, _value );
                }
                    , volume, _to, _time
                    , MENGINE_DOCUMENT_PYTHON
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( affector, "invalid create affector" );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                affectorHub->addAffector( affector );

                return soundIdentity;
            }
            //////////////////////////////////////////////////////////////////////////
            void soundStop( const SoundIdentityInterfacePtr & _identity )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid sound stop identity" );

                SOUND_SERVICE()
                    ->stopEmitter( _identity );
            }
            //////////////////////////////////////////////////////////////////////////
            void soundPause( const SoundIdentityInterfacePtr & _identity )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid sound pause identity" );

                SOUND_SERVICE()
                    ->pauseEmitter( _identity );
            }
            //////////////////////////////////////////////////////////////////////////
            void soundResume( const SoundIdentityInterfacePtr & _identity )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid sound resume identity" );

                SOUND_SERVICE()
                    ->resumeEmitter( _identity );
            }
            //////////////////////////////////////////////////////////////////////////
            bool isSoundStop( const SoundIdentityInterfacePtr & _identity )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid sound check stop identity" );

                return SOUND_SERVICE()
                    ->isEmitterStop( _identity );
            }
            //////////////////////////////////////////////////////////////////////////
            bool isSoundPlay( const SoundIdentityInterfacePtr & _identity )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid sound check play identity" );

                return SOUND_SERVICE()
                    ->isEmitterPlay( _identity );
            }
            //////////////////////////////////////////////////////////////////////////
            bool isSoundPause( const SoundIdentityInterfacePtr & _identity )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid sound check pause identity" );

                return SOUND_SERVICE()
                    ->isEmitterPause( _identity );
            }
            //////////////////////////////////////////////////////////////////////////
            void voiceStop( const SoundIdentityInterfacePtr & _identity )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid voice stop identity" );

                SOUND_SERVICE()
                    ->stopEmitter( _identity );
            }
            //////////////////////////////////////////////////////////////////////////
            void soundSourceSetVolume( const SoundIdentityInterfacePtr & _identity, float _volume )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid sound source set volume identity" );

                if( SOUND_SERVICE()
                    ->setSourceVolume( _identity, _volume, _volume, MENGINE_MIXER_VALUE_DEFAULT_SPEED, true ) == false )
                {
                    LOGGER_ERROR( "sound emitter [%u] invalid set source volume [%f]"
                        , _identity->getUniqueIdentity()
                        , _volume
                    );
                }
            }
            //////////////////////////////////////////////////////////////////////////
            float soundSourceGetVolume( const SoundIdentityInterfacePtr & _identity )
            {
                MENGINE_ASSERTION_MEMORY_PANIC( _identity, "invalid sound source get volume identity" );

                float volume = SOUND_SERVICE()
                    ->getSourceVolume( _identity );

                return volume;
            }
            //////////////////////////////////////////////////////////////////////////
            void soundSetVolume( float _volume )
            {
                SOUND_SERVICE()
                    ->setSoundVolume( STRINGIZE_STRING_LOCAL( "Generic" ), _volume, 0.f, MENGINE_MIXER_VALUE_DEFAULT_SPEED );
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
                    ->setCommonVolume( STRINGIZE_STRING_LOCAL( "Generic" ), _volume, _volume, MENGINE_MIXER_VALUE_DEFAULT_SPEED );
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
                    ->setVoiceVolume( STRINGIZE_STRING_LOCAL( "Generic" ), _volume, _volume, MENGINE_MIXER_VALUE_DEFAULT_SPEED );
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
                    ->setMute( STRINGIZE_STRING_LOCAL( "Generic" ), _mute );
            }
            //////////////////////////////////////////////////////////////////////////
            bool isMute()
            {
                bool mute = SOUND_SERVICE()
                    ->mixMute();

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

        pybind::interface_<SoundIdentityInterface, pybind::bases<Factorable>>( _kernel, "SoundIdentity" )
            .def( "isStreamable", &SoundIdentityInterface::getStreamable )
            .def( "getStreamable", &SoundIdentityInterface::getStreamable )
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
        pybind::def_functor( _kernel, "soundGetPosition", soundScriptMethod, &SoundScriptMethod::soundGetPosition );
        pybind::def_functor( _kernel, "soundSetPosition", soundScriptMethod, &SoundScriptMethod::soundSetPosition );
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

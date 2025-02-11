#include "AmplifierScriptEmbedding.h"

#include "Interface/AmplifierServiceInterface.h"

#include "Interface/SoundServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/AffectorCallbackInterface.h"
#include "Kernel/AffectorHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{    
    namespace
    {
        class AmplifierScriptMethod
            : public Factorable
        {
        public:
            AmplifierScriptMethod()
                : m_affectorMusicId( INVALID_UNIQUE_ID )
            {
            }

            ~AmplifierScriptMethod() override
            {
            }

        public:
            bool initialize()
            {
                m_factoryMusicAffectorCallback = Helper::makeFactoryPool<MusicAffectorCallback, 4>( MENGINE_DOCUMENT_FACTORABLE );

                m_affectorCreatorMusic = Helper::makeFactorableUnique<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float>>( MENGINE_DOCUMENT_FACTORABLE );
                m_affectorCreatorMusic->initialize();

                return true;
            }

            void finalize()
            {
                m_affectorCreatorMusic->finalize();
                m_affectorCreatorMusic = nullptr;

                MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMusicAffectorCallback );

                m_factoryMusicAffectorCallback = nullptr;
            }

        public:
            //////////////////////////////////////////////////////////////////////////
            class PythonAmplifierMusicCallback
                : public AmplifierMusicCallbackInterface
                , public PythonCallbackProvider
                , public Factorable
            {
                DECLARE_FACTORABLE( PythonAmplifierMusicCallback );

            public:
                PythonAmplifierMusicCallback( const pybind::object & _cbs, const pybind::args & _args )
                    : PythonCallbackProvider( _cbs, _args )
                {
                }

                ~PythonAmplifierMusicCallback() override
                {
                }

            protected:
                void onMusicPause( const SoundIdentityInterfacePtr & _identity ) override
                {
                    this->call_method( "onAmplifierMusicPause", _identity );
                }

                void onMusicResume( const SoundIdentityInterfacePtr & _identity ) override
                {
                    this->call_method( "onAmplifierMusicResume", _identity );
                }

                void onMusicStop( const SoundIdentityInterfacePtr & _identity ) override
                {
                    this->call_method( "onAmplifierMusicStop", _identity );
                }

                void onMusicEnd( const SoundIdentityInterfacePtr & _identity ) override
                {
                    this->call_method( "onAmplifierMusicEnd", _identity );
                }
            };
            //////////////////////////////////////////////////////////////////////////
            bool musicPlay( const ConstString & _resourceMusic, float _pos, bool _isLooped, const pybind::object & _cbs, const pybind::args & _args )
            {
                LOGGER_INFO( "amplifier", "[script] music play resource '%s' pos '%.2f' loop [%u]"
                    , _resourceMusic.c_str()
                    , _pos
                    , _isLooped
                );

                if( SERVICE_IS_INITIALIZE( AmplifierServiceInterface ) == false )
                {
                    return false;
                }

                AmplifierMusicCallbackInterfacePtr cb = Helper::makeFactorableUnique<PythonAmplifierMusicCallback>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );

                AMPLIFIER_SERVICE()
                    ->playMusic( _resourceMusic, _pos, _isLooped, cb );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            void musicSetVolume( float _volume )
            {
                SOUND_SERVICE()
                    ->setMusicVolume( STRINGIZE_STRING_LOCAL( "Generic" ), _volume, _volume, MENGINE_MIXER_VALUE_DEFAULT_SPEED );
            }
            //////////////////////////////////////////////////////////////////////////
            float musicGetVolume()
            {
                float volume = SOUND_SERVICE()
                    ->getMusicVolume( STRINGIZE_STRING_LOCAL( "Generic" ) );

                return volume;
            }
            //////////////////////////////////////////////////////////////////////////
            void musicSetVolumeTag( const ConstString & _tag, float _volume, float _from )
            {
                SOUND_SERVICE()
                    ->setMusicVolume( _tag, _volume, _from, MENGINE_MIXER_VALUE_DEFAULT_SPEED );
            }
            //////////////////////////////////////////////////////////////////////////
            float musicGetVolumeTag( const ConstString & _tag )
            {
                float volume = SOUND_SERVICE()
                    ->getMusicVolume( _tag );

                return volume;
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
                    ->setMusicVolume( STRINGIZE_STRING_LOCAL( "Fade" ), _volume, _volume, MENGINE_MIXER_VALUE_DEFAULT_SPEED );
            }
            //////////////////////////////////////////////////////////////////////////
            class MusicAffectorCallback
                : public AffectorCallbackInterface
                , public PythonCallbackProvider
                , public Factorable
            {
                DECLARE_FACTORABLE( MusicAffectorCallback );

            public:
                MusicAffectorCallback()
                {
                }

                ~MusicAffectorCallback() override
                {
                }

            protected:
                void onAffectorEnd( UniqueId _id, bool _isEnd ) override
                {
                    AMPLIFIER_SERVICE()
                        ->stopMusic();

                    this->call_cb( _id, _isEnd );
                }
            };
            //////////////////////////////////////////////////////////////////////////
            typedef IntrusivePtr<MusicAffectorCallback> MusicAffectorCallbackPtr;
            //////////////////////////////////////////////////////////////////////////
            FactoryInterfacePtr m_factoryMusicAffectorCallback;
            //////////////////////////////////////////////////////////////////////////
            MusicAffectorCallbackPtr createMusicAffectorCallback( const pybind::object & _cb, const pybind::args & _args )
            {
                MusicAffectorCallbackPtr callback = m_factoryMusicAffectorCallback->createObject( MENGINE_DOCUMENT_PYBIND );

                callback->initialize( _cb, _args );

                return callback;
            }
            //////////////////////////////////////////////////////////////////////////
            UniqueId m_affectorMusicId;
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float>> m_affectorCreatorMusic;
            //////////////////////////////////////////////////////////////////////////
            UniqueId musicFadeIn( float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                LOGGER_INFO( "amplifier", "[script] music fade in time '%.2f' easing '%s'"
                    , _time
                    , _easingType.c_str()
                );

                if( SERVICE_IS_INITIALIZE( AmplifierServiceInterface ) == false )
                {
                    return INVALID_UNIQUE_ID;
                }

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                MusicAffectorCallbackPtr callback = createMusicAffectorCallback( _cb, _args );

                AffectorPtr affector = m_affectorCreatorMusic->create( EAFFECTORTYPE_POSITION
                    , easing
                    , callback
                    , [this]( float _value )
                {
                    this->___musicFade( _value );
                }
                    , 1.f, 0.f, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                if( m_affectorMusicId != INVALID_UNIQUE_ID )
                {
                    if( affectorHub->hasAffector( m_affectorMusicId ) == true )
                    {
                        affectorHub->stopAffector( m_affectorMusicId );
                    }
                }

                UniqueId id = affectorHub->addAffector( affector );

                m_affectorMusicId = id;

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            UniqueId musicFadeOut( const ConstString & _resourceMusic, float _pos, bool _isLooped, float _time, const ConstString & _easingType, const pybind::object & _cbs, const pybind::args & _args )
            {
                LOGGER_INFO( "amplifier", "[script] music fade out resource '%s' pos '%.2f' loop [%u] time '%.2f' easing '%s'"
                    , _resourceMusic.c_str()
                    , _pos
                    , _isLooped
                    , _time
                    , _easingType.c_str()
                );

                if( SERVICE_IS_INITIALIZE( AmplifierServiceInterface ) == false )
                {
                    return INVALID_UNIQUE_ID;
                }

                AmplifierMusicCallbackInterfacePtr cb = Helper::makeFactorableUnique<PythonAmplifierMusicCallback>( MENGINE_DOCUMENT_PYBIND, _cbs, _args );

                if( AMPLIFIER_SERVICE()
                    ->playMusic( _resourceMusic, _pos, _isLooped, cb ) == false )
                {
                    LOGGER_ASSERTION( "invalid play music '%s' "
                        , _resourceMusic.c_str()
                    );

                    return INVALID_UNIQUE_ID;
                }

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                AffectorPtr affector = m_affectorCreatorMusic->create( EAFFECTORTYPE_POSITION
                    , easing
                    , nullptr
                    , [this]( float _value )
                {
                    this->___musicFade( _value );
                }
                    , 0.f, 1.f, _time
                    , MENGINE_DOCUMENT_PYBIND
                    );

                const AffectorHubInterfacePtr & affectorHub = PLAYER_SERVICE()
                    ->getGlobalAffectorHub();

                if( m_affectorMusicId != INVALID_UNIQUE_ID )
                {
                    if( affectorHub->hasAffector( m_affectorMusicId ) == true )
                    {
                        affectorHub->stopAffector( m_affectorMusicId );
                    }
                }

                UniqueId id = affectorHub->addAffector( affector );

                m_affectorMusicId = id;

                return id;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AmplifierScriptMethod> AmplifierScriptMethodPtr;
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AmplifierScriptEmbedding::AmplifierScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AmplifierScriptEmbedding::~AmplifierScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AmplifierScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        AmplifierScriptMethodPtr scriptMethod = Helper::makeFactorableUnique<AmplifierScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( scriptMethod->initialize() == false )
        {
            return false;
        }

        SCRIPT_SERVICE()
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("Amplifier"), true );

        pybind::def_functor_args( _kernel, "musicPlay", scriptMethod, &AmplifierScriptMethod::musicPlay );
        pybind::def_functor( _kernel, "musicSetVolume", scriptMethod, &AmplifierScriptMethod::musicSetVolume );
        pybind::def_functor( _kernel, "musicGetVolume", scriptMethod, &AmplifierScriptMethod::musicGetVolume );
        pybind::def_functor( _kernel, "musicSetVolumeTag", scriptMethod, &AmplifierScriptMethod::musicSetVolumeTag );
        pybind::def_functor( _kernel, "musicGetVolumeTag", scriptMethod, &AmplifierScriptMethod::musicGetVolumeTag );
        pybind::def_functor( _kernel, "musicStop", scriptMethod, &AmplifierScriptMethod::musicStop );
        pybind::def_functor( _kernel, "musicPause", scriptMethod, &AmplifierScriptMethod::musicPause );
        pybind::def_functor( _kernel, "musicResume", scriptMethod, &AmplifierScriptMethod::musicResume );
        pybind::def_functor( _kernel, "musicGetDuration", scriptMethod, &AmplifierScriptMethod::musicGetDuration );
        pybind::def_functor( _kernel, "musicGetLengthMs", scriptMethod, &AmplifierScriptMethod::musicGetDuration );
        pybind::def_functor( _kernel, "musicGetPosMs", scriptMethod, &AmplifierScriptMethod::musicGetPosMs );
        pybind::def_functor( _kernel, "musicSetPosMs", scriptMethod, &AmplifierScriptMethod::musicSetPosMs );
        pybind::def_functor_args( _kernel, "musicFadeIn", scriptMethod, &AmplifierScriptMethod::musicFadeIn );
        pybind::def_functor_args( _kernel, "musicFadeOut", scriptMethod, &AmplifierScriptMethod::musicFadeOut );

        m_implement = scriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        AmplifierScriptMethodPtr scriptMethod = m_implement;
        scriptMethod->finalize();

        m_implement = nullptr;
    }
}


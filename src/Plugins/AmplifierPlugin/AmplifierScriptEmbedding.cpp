#include "AmplifierScriptEmbedding.h"

#include "AmplifierInterface.h"

#include "Interface/SoundServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/AffectorCallbackInterface.h"
#include "Kernel/AffectorHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{    
    namespace
    {
        class AmplifierScriptMethod
            : public Factorable
        {
        public:
            AmplifierScriptMethod()
                : m_affectorMusicID( INVALID_UNIQUE_ID )
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
                if( SERVICE_IS_INITIALIZE( AmplifierInterface ) == false )
                {
                    return;
                }

                AmplifierMusicCallbackInterfacePtr cb = nullptr;

                if( _cb.is_callable() == true )
                {
                    cb = Helper::makeFactorableUnique<PythonAmplifierMusicCallback>( MENGINE_DOCUMENT_PYBIND, _cb, _args );
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
                DECLARE_FACTORABLE( MusicAffectorCallback );

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
            FactoryInterfacePtr m_factoryMusicAffectorCallback;
            //////////////////////////////////////////////////////////////////////////
            MusicAffectorCallbackPtr createMusicAffectorCallback( const pybind::object & _cb, const pybind::args & _args )
            {
                MusicAffectorCallbackPtr callback = m_factoryMusicAffectorCallback->createObject( MENGINE_DOCUMENT_PYBIND );
                callback->initialize( _cb, _args );

                return callback;
            }
            //////////////////////////////////////////////////////////////////////////
            UniqueId m_affectorMusicID;
            //////////////////////////////////////////////////////////////////////////
            IntrusivePtr<NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float>> m_affectorCreatorMusic;
            //////////////////////////////////////////////////////////////////////////
            uint32_t musicFadeIn( float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                if( SERVICE_IS_INITIALIZE( AmplifierInterface ) == false )
                {
                    return 0;
                }

                EasingInterfacePtr easing = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Easing" ), _easingType );

                MusicAffectorCallbackPtr callback = createMusicAffectorCallback( _cb, _args );

                AffectorPtr affector = m_affectorCreatorMusic->create( ETA_POSITION
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

                if( m_affectorMusicID != INVALID_UNIQUE_ID )
                {
                    if( affectorHub->hasAffector( m_affectorMusicID ) == true )
                    {
                        affectorHub->stopAffector( m_affectorMusicID );
                    }
                }

                UniqueId id = affectorHub->addAffector( affector );

                m_affectorMusicID = id;

                return id;
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t musicFadeOut( const ConstString & _resourceMusic, float _pos, bool _isLooped, float _time, const ConstString & _easingType, const pybind::object & _cb, const pybind::args & _args )
            {
                if( SERVICE_IS_INITIALIZE( AmplifierInterface ) == false )
                {
                    return 0;
                }

                AmplifierMusicCallbackInterfacePtr cb = nullptr;

                if( _cb.is_callable() == true )
                {
                    cb = Helper::makeFactorableUnique<PythonAmplifierMusicCallback>( MENGINE_DOCUMENT_PYBIND, _cb, _args );
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

                AffectorPtr affector = m_affectorCreatorMusic->create( ETA_POSITION
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

                if( m_affectorMusicID != INVALID_UNIQUE_ID )
                {
                    if( affectorHub->hasAffector( m_affectorMusicID ) == true )
                    {
                        affectorHub->stopAffector( m_affectorMusicID );
                    }
                }

                UniqueId id = affectorHub->addAffector( affector );

                m_affectorMusicID = id;

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
            ->setAvailablePlugin( "Amplifier", true );

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


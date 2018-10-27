#include "PythonWrapper.h"

#include "Interface/AmplifierInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PlayerInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/Affectorable.h"
#include "Kernel/Affector.h"
#include "Kernel/AffectorHelper.h"

#include "Config/Lambda.h"

#include "Engine/ResourceSound.h"
#include "Kernel/FactoryPool.h"

#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

#include <math.h>

namespace Mengine
{
    class SoundScriptMethod
    {
    public:
        SoundScriptMethod()
        {
            m_factorySoundAffectorCallback = new FactoryPool<SoundAffectorCallback, 4>();
            m_factoryMusicAffectorCallback = new FactoryPool<MusicAffectorCallback, 4>();
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        class MySoundNodeListener
            : public FactorableUnique<Factorable>
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
                    m_resource->decrementReference();
                    m_resource = nullptr;
                }
            }

        protected:
            void onSoundPause( const SoundIdentityInterfacePtr & _emitter ) override
            {
                (void)_emitter;
                //Empty
            }

            void onSoundResume( const SoundIdentityInterfacePtr & _emitter ) override
            {
                (void)_emitter;
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

                    LOGGER_ERROR( "MySoundNodeListenerInterface '%s' emitter invalid release sound '%d'"
                        , m_resource->getName().c_str()
                        , id
                    );
                }

                m_resource->decrementReference();
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

                    LOGGER_ERROR( "MySoundNodeListenerInterface '%s' emitter invalid release sound '%d'"
                        , m_resource->getName().c_str()
                        , id
                    );
                }

                m_resource->decrementReference();
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
            ResourceSoundPtr resource = RESOURCE_SERVICE()
                ->getResource( _resourceName );

            if( resource == nullptr )
            {
                return nullptr;
            }

            SoundBufferInterfacePtr soundBuffer = resource->createSoundBuffer();

            if( soundBuffer == nullptr )
            {
                resource->decrementReference();

                return nullptr;
            }

            bool streamable = resource->isStreamable();

            SoundIdentityInterfacePtr sourceEmitter = SOUND_SERVICE()
                ->createSoundIdentity( true, soundBuffer, _category, streamable );

            if( sourceEmitter == nullptr )
            {
                resource->decrementReference();

                return nullptr;
            }

            SOUND_SERVICE()
                ->setLoop( sourceEmitter, _loop );

            float volume = resource->getDefaultVolume();

            if( SOUND_SERVICE()
                ->setSourceVolume( sourceEmitter, volume, volume, true ) == false )
            {
                LOGGER_ERROR( "ScriptWrapper::createSoundSource invalid %s setSourceVolume %f"
                    , _resourceName.c_str()
                    , volume
                );

                resource->decrementReference();

                return nullptr;
            }

            MySoundNodeListenerPtr snlistener = new MySoundNodeListener( resource, soundBuffer, _cb, _args );

            sourceEmitter->setSoundListener( snlistener );

            return sourceEmitter;
        }
        //////////////////////////////////////////////////////////////////////////
        SoundIdentityInterfacePtr soundPlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb, const pybind::args & _args )
        {
            SoundIdentityInterfacePtr sourceEmitter = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cb, _args );

            if( sourceEmitter == nullptr )
            {
                LOGGER_ERROR( "soundPlay: can't get resource '%s'"
                    , _resourceName.c_str()
                );

                return nullptr;
            }

            if( SOUND_SERVICE()
                ->playEmitter( sourceEmitter ) == false )
            {
                LOGGER_ERROR( "soundPlay: invalid play '%s'"
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

            if( sourceEmitter == nullptr )
            {
                LOGGER_ERROR( "voicePlay: can't get resource '%s'"
                    , _resourceName.c_str()
                );

                return nullptr;
            }

            if( SOUND_SERVICE()
                ->playEmitter( sourceEmitter ) == false )
            {
                LOGGER_ERROR( "voicePlay: invalid play '%s'"
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

            if( sourceEmitter == nullptr )
            {
                LOGGER_ERROR( "soundPlayFromPosition: can't get resource '%s'"
                    , _resourceName.c_str()
                );

                return nullptr;
            }

            if( SOUND_SERVICE()
                ->setPosMs( sourceEmitter, _position ) == false )
            {
                LOGGER_ERROR( "soundPlayFromPosition: set pos '%s' '%f'"
                    , _resourceName.c_str()
                    , _position
                );

                return nullptr;
            }

            if( SOUND_SERVICE()
                ->playEmitter( sourceEmitter ) == false )
            {
                LOGGER_ERROR( "soundPlayFromPosition: play '%s' '%f'"
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
            SoundAffectorCallbackPtr callback = m_factorySoundAffectorCallback->createObject();

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
        void soundFadeIn( const SoundIdentityInterfacePtr & _emitter, float _time, const pybind::object & _cb, const pybind::args & _args )
        {
            SoundAffectorCallbackPtr callback = createSoundAffectorCallback( _emitter, _cb, _args );

            AffectorPtr affector =
                m_affectorCreatorSound.create( ETA_POSITION
                    , callback, [this, _emitter]( float _volume ) { this->___soundFade( _emitter, _volume ); }
                    , 1.f, 0.f, _time
                );

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            affectorable->addAffector( affector );
        }
        //////////////////////////////////////////////////////////////////////////
        SoundIdentityInterfacePtr soundFadeOut( const ConstString & _resourceName, bool _loop, float _time, const pybind::object & _cb, const pybind::args & _args )
        {
            SoundIdentityInterfacePtr sourceEmitter = s_createSoundSource( _resourceName, _loop, ES_SOURCE_CATEGORY_SOUND, _cb, _args );

            if( sourceEmitter == nullptr )
            {
                LOGGER_ERROR( "soundPlay: can't get resource '%s'"
                    , _resourceName.c_str()
                );

                return nullptr;
            }

            if( SOUND_SERVICE()
                ->playEmitter( sourceEmitter ) == false )
            {
                LOGGER_ERROR( "soundPlay: invalid play '%s'"
                    , _resourceName.c_str()
                );

                return nullptr;
            }

            AffectorPtr affector =
                m_affectorCreatorSound.create( ETA_POSITION
                    , nullptr, [this, sourceEmitter]( float _value ) { this->___soundFade( sourceEmitter, _value ); }
                    , 0.f, 1.f, _time
                );

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
                LOGGER_ERROR( "SoundScriptWrapper::s_soundSourceSetVolume invalid source volume %d"
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
            : public FactorableUnique<AmplifierMusicCallbackInterface>
        {
        public:
            PythonAmplifierMusicCallback( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {
            }

        protected:
            void onMusicPause() override
            {
                m_cb.call_args( 0, m_args );
            }

            void onMusicResume()
            {
                m_cb.call_args( 1, m_args );
            }
            
            void onMusicStop()
            {
                m_cb.call_args( 2, m_args );
            }

            void onMusicEnd()
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
                cb = new PythonAmplifierMusicCallback( _cb, _args );
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
            MusicAffectorCallbackPtr callback = m_factoryMusicAffectorCallback->createObject();

            callback->initialize( _cb, _args );

            return callback;
        }
        //////////////////////////////////////////////////////////////////////////		
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float> m_affectorCreatorMusic;
        //////////////////////////////////////////////////////////////////////////
        uint32_t musicFadeIn( float _time, const pybind::object & _cb, const pybind::args & _args )
        {
            if( SERVICE_EXIST( AmplifierInterface ) == false )
            {
                return 0;
            }

            MusicAffectorCallbackPtr callback = createMusicAffectorCallback( _cb, _args );

            AffectorPtr affector =
                m_affectorCreatorMusic.create( ETA_POSITION
                    , callback
                    , [this]( float _value ) { this->___musicFade( _value ); }
                    , 1.f, 0.f, _time
                );

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            AFFECTOR_ID id = affectorable->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t musicFadeOut( const ConstString & _resourceMusic, float _pos, bool _isLooped, float _time, const pybind::object & _cb, const pybind::args & _args )
        {
            if( SERVICE_EXIST( AmplifierInterface ) == false )
            {
                return 0;
            }

            AmplifierMusicCallbackInterfacePtr cb = nullptr;

            if( _cb.is_callable() == true )
            {
                cb = new PythonAmplifierMusicCallback( _cb, _args );
            }

            if( AMPLIFIER_SERVICE()
                ->playMusic( _resourceMusic, _pos, _isLooped, cb ) == false )
            {
                LOGGER_ERROR( "AMPLIFIER_SERVICE: playMusic '%s' invalid"
                    , _resourceMusic.c_str()
                );

                return 0;
            }

            AffectorPtr affector =
                m_affectorCreatorMusic.create( ETA_POSITION
                    , nullptr
                    , [this]( float _value ) { this->___musicFade( _value ); }
                    , 0.f, 1.f, _time
                );

            const AffectorablePtr & affectorable = PLAYER_SERVICE()
                ->getGlobalAffectorable();

            AFFECTOR_ID id = affectorable->addAffector( affector );

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
    bool PythonWrapper::soundWrap()
    {
        SoundScriptMethod * soundScriptMethod = new SoundScriptMethod();

        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::interface_<SoundIdentityInterface, pybind::bases<Mixin> >( kernel, "SoundIdentity" )
            .def( "getId", &SoundIdentityInterface::getId )
            .def( "isStreamable", &SoundIdentityInterface::isStreamable )
            .def( "getLoop", &SoundIdentityInterface::getLoop )
            ;

        pybind::def_functor( kernel, "hasSound", soundScriptMethod, &SoundScriptMethod::hasSound );
        pybind::def_functor( kernel, "isMute", soundScriptMethod, &SoundScriptMethod::isMute );
        pybind::def_functor( kernel, "isSilent", soundScriptMethod, &SoundScriptMethod::isSilent );
        pybind::def_functor( kernel, "soundMute", soundScriptMethod, &SoundScriptMethod::soundMute );

        pybind::def_functor( kernel, "commonSetVolume", soundScriptMethod, &SoundScriptMethod::commonSetVolume );
        pybind::def_functor( kernel, "commonGetVolume", soundScriptMethod, &SoundScriptMethod::commonGetVolume );

        pybind::def_functor_args( kernel, "soundPlay", soundScriptMethod, &SoundScriptMethod::soundPlay );
        pybind::def_functor_args( kernel, "soundPlayFromPosition", soundScriptMethod, &SoundScriptMethod::soundPlayFromPosition );
        pybind::def_functor( kernel, "soundStop", soundScriptMethod, &SoundScriptMethod::soundStop );
        pybind::def_functor( kernel, "soundPause", soundScriptMethod, &SoundScriptMethod::soundPause );
        pybind::def_functor( kernel, "soundResume", soundScriptMethod, &SoundScriptMethod::soundResume );
        pybind::def_functor( kernel, "soundSourceSetVolume", soundScriptMethod, &SoundScriptMethod::soundSourceSetVolume );
        pybind::def_functor( kernel, "soundSourceGetVolume", soundScriptMethod, &SoundScriptMethod::soundSourceGetVolume );
        pybind::def_functor( kernel, "soundSetVolume", soundScriptMethod, &SoundScriptMethod::soundSetVolume );
        pybind::def_functor( kernel, "soundGetVolume", soundScriptMethod, &SoundScriptMethod::soundGetVolume );
        pybind::def_functor( kernel, "soundGetPosition", soundScriptMethod, &SoundScriptMethod::soundGetPosMs );
        pybind::def_functor( kernel, "soundSetPosition", soundScriptMethod, &SoundScriptMethod::soundSetPosMs );
        pybind::def_functor_args( kernel, "soundFadeIn", soundScriptMethod, &SoundScriptMethod::soundFadeIn );
        pybind::def_functor_args( kernel, "soundFadeOut", soundScriptMethod, &SoundScriptMethod::soundFadeOut );

        pybind::def_functor_args( kernel, "musicPlay", soundScriptMethod, &SoundScriptMethod::musicPlay );
        pybind::def_functor( kernel, "musicSetVolume", soundScriptMethod, &SoundScriptMethod::musicSetVolume );
        pybind::def_functor( kernel, "musicGetVolume", soundScriptMethod, &SoundScriptMethod::musicGetVolume );
        pybind::def_functor( kernel, "musicSetVolumeTag", soundScriptMethod, &SoundScriptMethod::musicSetVolumeTag );
        pybind::def_functor( kernel, "musicGetVolumeTag", soundScriptMethod, &SoundScriptMethod::musicGetVolumeTag );
        pybind::def_functor( kernel, "musicStop", soundScriptMethod, &SoundScriptMethod::musicStop );
        pybind::def_functor( kernel, "musicPause", soundScriptMethod, &SoundScriptMethod::musicPause );
        pybind::def_functor( kernel, "musicResume", soundScriptMethod, &SoundScriptMethod::musicResume );
        pybind::def_functor( kernel, "musicGetDuration", soundScriptMethod, &SoundScriptMethod::musicGetDuration );
        pybind::def_functor( kernel, "musicGetLengthMs", soundScriptMethod, &SoundScriptMethod::musicGetDuration );
        pybind::def_functor( kernel, "musicGetPosMs", soundScriptMethod, &SoundScriptMethod::musicGetPosMs );
        pybind::def_functor( kernel, "musicSetPosMs", soundScriptMethod, &SoundScriptMethod::musicSetPosMs );
        pybind::def_functor_args( kernel, "musicFadeIn", soundScriptMethod, &SoundScriptMethod::musicFadeIn );
        pybind::def_functor_args( kernel, "musicFadeOut", soundScriptMethod, &SoundScriptMethod::musicFadeOut );


        pybind::def_functor_args( kernel, "voicePlay", soundScriptMethod, &SoundScriptMethod::voicePlay );
        pybind::def_functor( kernel, "voiceStop", soundScriptMethod, &SoundScriptMethod::voiceStop );
        pybind::def_functor( kernel, "voicePause", soundScriptMethod, &SoundScriptMethod::voicePause );
        pybind::def_functor( kernel, "voiceResume", soundScriptMethod, &SoundScriptMethod::voiceResume );
        pybind::def_functor( kernel, "voiceSetVolume", soundScriptMethod, &SoundScriptMethod::voiceSetVolume );
        pybind::def_functor( kernel, "voiceGetVolume", soundScriptMethod, &SoundScriptMethod::voiceGetVolume );

        return true;
    }
}

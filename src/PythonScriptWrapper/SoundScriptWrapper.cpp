#	include "PythonScriptWrapper.h"

#	include "Interface/AmplifierInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/PlayerInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Kernel/Affectorable.h"

#	include "Menge/ResourceSound.h"

#	include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

#	include <math.h>

namespace	Menge
{
	class SoundScriptMethod
	{
    public:
        SoundScriptMethod( ServiceProviderInterface * _serviceProvider )
            : m_serviceProvider(_serviceProvider)
        {
        }

    public:
		//////////////////////////////////////////////////////////////////////////
		class MySoundNodeListenerInterface
			: public SoundListenerInterface
		{
		public:
			MySoundNodeListenerInterface()
			{
			}

			~MySoundNodeListenerInterface()
			{				
			}

		public:
			bool initialize( ServiceProviderInterface * _serviceProvider, const ResourceSoundPtr & _resource, const SoundBufferInterfacePtr & _soundBuffer, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
			{
				m_serviceProvider = _serviceProvider;
				m_resource = _resource;
				m_soundBuffer = _soundBuffer;

				m_cb = _cb;				
				m_args = _args;

				return true;
			}

		protected:
			void onSoundPause( uint32_t _id ) override
			{
				(void)_id;
				//Empty
			}

			void onSoundStop( uint32_t _id ) override
			{	
				if( m_cb.is_callable() == true )
				{
					m_cb.call_args( _id, m_args );
				}

				if( SOUND_SERVICE(m_serviceProvider)
					->releaseSoundSource( _id ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("MySoundNodeListenerInterface %s emitter invalid release sound %d"
						, m_resource->getName().c_str()
						, _id
						);
				}
				         
				m_resource->decrementReference();
				m_resource = nullptr;

				m_soundBuffer = nullptr;

				delete this;
			}

		protected:
            ServiceProviderInterface * m_serviceProvider;
			ResourceSoundPtr m_resource;
            SoundBufferInterfacePtr m_soundBuffer;
			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		bool s_hasSound( const ConstString & _resourceName )
		{
			if( RESOURCE_SERVICE(m_serviceProvider)
				->validResourceType( _resourceName, STRINGIZE_STRING_LOCAL(m_serviceProvider, "ResourceSound") ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_createSoundSource( const ConstString & _resourceName, bool _loop, ESoundSourceType _type, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			ResourceSoundPtr resource = RESOURCE_SERVICE(m_serviceProvider)
				->getResourceT<ResourceSoundPtr>( _resourceName );

			if( resource == nullptr )
			{
				return 0;
			}

			SoundBufferInterfacePtr soundBuffer = resource->createSoundBuffer();

            if( soundBuffer == nullptr )
            {
				resource->decrementReference();

                return 0;
            }

			bool streamable = resource->isStreamable();

			uint32_t sourceID = SOUND_SERVICE(m_serviceProvider)
                ->createSoundSource( true, soundBuffer, _type, streamable );

			if( sourceID == 0 )
			{
                resource->decrementReference();
                
				return 0;
			}

			SOUND_SERVICE(m_serviceProvider)
				->setLoop( sourceID, _loop );

			float volume = resource->getDefaultVolume();

			if( SOUND_SERVICE(m_serviceProvider)
				->setSourceVolume( sourceID, volume, volume ) == false )
			{                
				LOGGER_ERROR(m_serviceProvider)("ScriptWrapper::createSoundSource invalid %s setSourceVolume %f"
					, _resourceName.c_str()
                    , volume
					);

                resource->decrementReference();

                return 0;
			}


			MySoundNodeListenerInterface * snlistener = new MySoundNodeListenerInterface();

			if( snlistener->initialize( m_serviceProvider, resource, soundBuffer, _cb, _args ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptWrapper::createSoundSource invalid %s intialize listener"
					, _resourceName.c_str()
					, volume
					);

				delete snlistener;

				resource->decrementReference();

				return 0;
			}

			SOUND_SERVICE(m_serviceProvider)
				->setSourceListener( sourceID, snlistener );
			
			return sourceID;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_soundPlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			uint32_t sourceID = s_createSoundSource( _resourceName, _loop, ESST_SOUND, _cb, _args );

			if( sourceID == 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("soundPlay: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			if( SOUND_SERVICE(m_serviceProvider)
				->play( sourceID ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("soundPlay: invalid play '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}
              
			return sourceID;
		}
        //////////////////////////////////////////////////////////////////////////
		uint32_t s_voicePlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
        {
            uint32_t sourceID = s_createSoundSource( _resourceName, _loop, ESST_VOICE, _cb, _args );

            if( sourceID == 0 )
            {
                LOGGER_ERROR(m_serviceProvider)("voicePlay: can't get resource '%s'"
                    , _resourceName.c_str()
                    );

                return 0;
            }

            if( SOUND_SERVICE(m_serviceProvider)
                ->play( sourceID ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("voicePlay: invalid play '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

            return sourceID;
        }
		//////////////////////////////////////////////////////////////////////////
		bool s_voicePause( uint32_t _soundId )
		{
			bool successful = SOUND_SERVICE( m_serviceProvider )
				->pause( _soundId );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_voiceResume( uint32_t _soundId )
		{
			bool successful = SOUND_SERVICE( m_serviceProvider )
				->resume( _soundId );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_soundPlayFromPosition( const ConstString & _resourceName, float _position, bool _loop, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			uint32_t sourceID = s_createSoundSource( _resourceName, _loop, ESST_SOUND, _cb, _args );

			if( sourceID == 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("soundPlayFromPosition: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			if( SOUND_SERVICE(m_serviceProvider)
				->setPosMs( sourceID, _position ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("soundPlayFromPosition: set pos '%s' '%f'"
                    , _resourceName.c_str()
                    , _position
                    );

                return 0;
            }
			
			if( SOUND_SERVICE(m_serviceProvider)
				->play( sourceID ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("soundPlayFromPosition: play '%s' '%f'"
                    , _resourceName.c_str()
                    , _position
                    );

                return 0;
            }

			return sourceID;
		}
		//////////////////////////////////////////////////////////////////////////
		float s_soundGetPosMs( uint32_t _sourceID )
		{
			float pos =	SOUND_SERVICE(m_serviceProvider)
				->getPosMs( _sourceID );

			return pos;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundSetPosMs( uint32_t _sourceID, float _pos )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setPosMs( _sourceID, _pos );
		}
		//////////////////////////////////////////////////////////////////////////
		class SoundAffectorCallback
			: public AffectorCallback
		{
		public:
			SoundAffectorCallback()
				: m_serviceProvider( nullptr )
				, m_sourceId(0)
			{
			}

			~SoundAffectorCallback()
			{
			}

		public:
			void initialize( ServiceProviderInterface * _serviceProvider, uint32_t _sourceId, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
			{
				m_serviceProvider = _serviceProvider;
				m_sourceId = _sourceId;
				m_cb = _cb;
				m_args = _args;
			}

		protected:
			void onAffectorEnd( uint32_t _id, bool _isEnd ) override
			{
				if( _isEnd == true )
				{
					SOUND_SERVICE( m_serviceProvider )
						->stop( m_sourceId );
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
			ServiceProviderInterface * m_serviceProvider;
			uint32_t m_sourceId;
			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPoolStore<SoundAffectorCallback, 4> m_factorySoundAffectorCallback;
		//////////////////////////////////////////////////////////////////////////
		SoundAffectorCallback * createSoundAffectorCallback( uint32_t _sourceId, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			SoundAffectorCallback * callback = m_factorySoundAffectorCallback.createObject();

			callback->initialize( m_serviceProvider, _sourceId, _cb, _args );

			return callback;
		}
		//////////////////////////////////////////////////////////////////////////
		void ___soundFade( uint32_t _sourceId, float _volume )
		{
			SOUND_SERVICE( m_serviceProvider )
				->setSourceMixerVolume( _sourceId, STRINGIZE_STRING_LOCAL( m_serviceProvider, "Fade" ), _volume, _volume );
		}
		//////////////////////////////////////////////////////////////////////////		
		NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<SoundScriptMethod, void (SoundScriptMethod::*)(uint32_t, float), float, uint32_t> m_affectorCreatorSound;
		//////////////////////////////////////////////////////////////////////////
		void s_soundFadeIn( uint32_t _sourceId, float _time, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			SoundAffectorCallback * callback = createSoundAffectorCallback( _sourceId, _cb, _args );

			Affector* affector =
				m_affectorCreatorSound.create( m_serviceProvider
				, ETA_POSITION, callback, this, &SoundScriptMethod::___soundFade, _sourceId
				, 1.f, 0.f, _time
				);

			Affectorable * affectorable = PLAYER_SERVICE( m_serviceProvider )
				->getAffectorableGlobal();

			affectorable->addAffector( affector );
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_soundFadeOut( const ConstString & _resourceName, bool _loop, float _time, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			uint32_t sourceId = s_createSoundSource( _resourceName, _loop, ESST_SOUND, _cb, _args );

			if( sourceId == 0 )
			{
				LOGGER_ERROR( m_serviceProvider )("soundPlay: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			if( SOUND_SERVICE( m_serviceProvider )
				->play( sourceId ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("soundPlay: invalid play '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			Affector* affector =
				m_affectorCreatorSound.create( m_serviceProvider
				, ETA_POSITION, nullptr, this, &SoundScriptMethod::___soundFade, sourceId
				, 0.f, 1.f, _time
				);

			Affectorable * affectorable = PLAYER_SERVICE( m_serviceProvider )
				->getAffectorableGlobal();

			affectorable->addAffector( affector );

			return sourceId;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundStop( uint32_t _sourceID )
		{
			SOUND_SERVICE(m_serviceProvider)
				->stop( _sourceID );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundPause( uint32_t _sourceID )
		{
			SOUND_SERVICE( m_serviceProvider )
				->pause( _sourceID );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundResume( uint32_t _sourceID )
		{
			SOUND_SERVICE( m_serviceProvider )
				->resume( _sourceID );
		}
		//////////////////////////////////////////////////////////////////////////
        void s_voiceStop( uint32_t _sourceID )
        {
            SOUND_SERVICE(m_serviceProvider)
                ->stop( _sourceID );
        }
		//////////////////////////////////////////////////////////////////////////
		void s_soundSourceSetVolume( uint32_t _sourceID, float _volume )
		{
			if( SOUND_SERVICE(m_serviceProvider)
				->setSourceVolume( _sourceID, _volume, _volume ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("SoundScriptWrapper::s_soundSourceSetVolume invalid source volume %d"
					, _sourceID
					);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		float s_soundSourceGetVolume( uint32_t _sourceID )
		{
			float volume = SOUND_SERVICE(m_serviceProvider)
				->getSourceVolume( _sourceID );

            return volume;                 
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundSetVolume( float _volume )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setSoundVolume(STRINGIZE_STRING_LOCAL(m_serviceProvider, "Generic"), _volume, 0.f );
		}
		//////////////////////////////////////////////////////////////////////////
		float s_soundGetVolume()
		{
			float volume = SOUND_SERVICE(m_serviceProvider)
				->getSoundVolume(STRINGIZE_STRING_LOCAL(m_serviceProvider, "Generic") );

            return volume;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_commonSetVolume( float _volume )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setCommonVolume(STRINGIZE_STRING_LOCAL( m_serviceProvider, "Generic" ), _volume, _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float commonGetVolume()
		{
			return SOUND_SERVICE(m_serviceProvider)
				->getCommonVolume(STRINGIZE_STRING_LOCAL(m_serviceProvider, "Generic") );
		}
		//////////////////////////////////////////////////////////////////////////
		void musicPlay( const ConstString & _resourceMusic, float _pos, bool _isLooped )
		{
			if( SERVICE_EXIST( m_serviceProvider, AmplifierInterface ) == false )
			{
				return;
			}

			AMPLIFIER_SERVICE(m_serviceProvider)
				->playMusic( _resourceMusic, _pos, _isLooped );
		}
		//////////////////////////////////////////////////////////////////////////
		void musicSetVolume( float _volume )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setMusicVolume(STRINGIZE_STRING_LOCAL(m_serviceProvider, "Generic"), _volume, _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float musicGetVolume()
		{
			return SOUND_SERVICE(m_serviceProvider)
				->getMusicVolume(STRINGIZE_STRING_LOCAL(m_serviceProvider, "Generic") );
		}
		//////////////////////////////////////////////////////////////////////////
		void musicSetVolumeTag( const ConstString & _tag, float _volume, float _default )
		{
			SOUND_SERVICE( m_serviceProvider )
				->setMusicVolume( _tag, _volume, _default );
		}		
		//////////////////////////////////////////////////////////////////////////
		float musicGetVolumeTag( const ConstString & _tag )
		{
			return SOUND_SERVICE( m_serviceProvider )
				->getMusicVolume( _tag );
		}
        //////////////////////////////////////////////////////////////////////////
        void voiceSetVolume( float _volume )
        {
            SOUND_SERVICE(m_serviceProvider)
				->setVoiceVolume(STRINGIZE_STRING_LOCAL( m_serviceProvider, "Generic" ), _volume, _volume );
        }
        //////////////////////////////////////////////////////////////////////////
        float voiceGetVolume()
        {
            return SOUND_SERVICE(m_serviceProvider)
                ->getVoiceVolume(STRINGIZE_STRING_LOCAL(m_serviceProvider, "Generic") );
        }
		//////////////////////////////////////////////////////////////////////////
		void musicStop()
		{
			AMPLIFIER_SERVICE(m_serviceProvider)
				->stop();
		}
        //////////////////////////////////////////////////////////////////////////
        bool musicPause()
        {
            bool successful = AMPLIFIER_SERVICE(m_serviceProvider)
                ->pause();

			return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool musicResume()
        {
            bool successful = AMPLIFIER_SERVICE(m_serviceProvider)
                ->resume();

			return successful;
        }
		//////////////////////////////////////////////////////////////////////////
		float s_musicGetDuration()
		{
			float posMs = AMPLIFIER_SERVICE( m_serviceProvider )
				->getDuration();

			return posMs;
		}
		//////////////////////////////////////////////////////////////////////////
		float s_musicGetPosMs()
		{
			float posMs = AMPLIFIER_SERVICE(m_serviceProvider)
				->getPosMs();

			return posMs;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_musicSetPosMs( float _posMs )
		{
			AMPLIFIER_SERVICE(m_serviceProvider)
				->setPosMs( _posMs );
		}
		//////////////////////////////////////////////////////////////////////////
		void ___musicFade( float _volume )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setMusicVolume(STRINGIZE_STRING_LOCAL( m_serviceProvider, "Fade" ), _volume, _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		class MusicAffectorCallback
			: public AffectorCallback
		{
		public:
			MusicAffectorCallback()
				: m_serviceProvider(nullptr)
			{
			}

			~MusicAffectorCallback()
			{
			}

		public:
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
			{
				m_serviceProvider = _serviceProvider;
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

				AMPLIFIER_SERVICE( m_serviceProvider )
					->stop();

				m_cb.call_args( _id, _isEnd, m_args );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPoolStore<MusicAffectorCallback, 4> m_factoryMusicAffectorCallback;
		//////////////////////////////////////////////////////////////////////////
		MusicAffectorCallback * createMusicAffectorCallback( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			MusicAffectorCallback * callback = m_factoryMusicAffectorCallback.createObject();

			callback->initialize( m_serviceProvider, _cb, _args );

			return callback; 
		}
		//////////////////////////////////////////////////////////////////////////		
		NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<SoundScriptMethod, void (SoundScriptMethod::*)(float), float> m_affectorCreatorMusic;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_musicFadeIn( float _time, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			if( SERVICE_EXIST( m_serviceProvider, AmplifierInterface ) == false )
			{
				return 0;
			}

			MusicAffectorCallback * callback = createMusicAffectorCallback( _cb, _args );

			Affector* affector = 
				m_affectorCreatorMusic.create( m_serviceProvider
				, ETA_POSITION, callback, this, &SoundScriptMethod::___musicFade
				, 1.f, 0.f, _time
				);

			Affectorable * affectorable = PLAYER_SERVICE(m_serviceProvider)
				->getAffectorableGlobal();

			AFFECTOR_ID id = affectorable->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_musicFadeOut( const ConstString & _resourceMusic, float _pos, bool _isLooped, float _time )
		{
			if( SERVICE_EXIST( m_serviceProvider, AmplifierInterface ) == false )
			{
				return 0;
			}

			AMPLIFIER_SERVICE( m_serviceProvider )
				->playMusic( _resourceMusic, _pos, _isLooped );

			Affector* affector = 
				m_affectorCreatorMusic.create( m_serviceProvider
				, ETA_POSITION, nullptr, this, &SoundScriptMethod::___musicFade
				, 0.f, 1.f, _time
				);

			Affectorable * affectorable = PLAYER_SERVICE(m_serviceProvider)
				->getAffectorableGlobal();

			AFFECTOR_ID id = affectorable->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundMute( bool _mute )
		{
			SOUND_SERVICE(m_serviceProvider)
				->mute( _mute );
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_isMute()
		{
			bool mute = SOUND_SERVICE(m_serviceProvider)
				->isMute();

			return mute;
		}
        //////////////////////////////////////////////////////////////////////////
        bool s_isSilent()
        {
            bool silent = SOUND_SYSTEM(m_serviceProvider)
                ->isSilent();

            return silent;
        }

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};

	//////////////////////////////////////////////////////////////////////////
	//REGISTER_SCRIPT_CLASS( Menge, ScriptSoundHelper, Base )
	void PythonScriptWrapper::soundWrap( ServiceProviderInterface * _serviceProvider )
	{
        SoundScriptMethod * soundScriptMethod = new SoundScriptMethod(_serviceProvider);

		pybind::def_functor( "hasSound", soundScriptMethod, &SoundScriptMethod::s_hasSound );
        pybind::def_functor( "isMute", soundScriptMethod, &SoundScriptMethod::s_isMute );
        pybind::def_functor( "isSilent", soundScriptMethod, &SoundScriptMethod::s_isSilent );
        pybind::def_functor( "soundMute", soundScriptMethod, &SoundScriptMethod::s_soundMute );

        pybind::def_functor( "commonSetVolume", soundScriptMethod, &SoundScriptMethod::s_commonSetVolume );
        pybind::def_functor( "commonGetVolume", soundScriptMethod, &SoundScriptMethod::commonGetVolume );
                
        pybind::def_functor_args( "soundPlay", soundScriptMethod, &SoundScriptMethod::s_soundPlay );
		pybind::def_functor_args( "soundPlayFromPosition", soundScriptMethod, &SoundScriptMethod::s_soundPlayFromPosition );
		pybind::def_functor( "soundStop", soundScriptMethod, &SoundScriptMethod::s_soundStop );
		pybind::def_functor( "soundPause", soundScriptMethod, &SoundScriptMethod::s_soundPause );
		pybind::def_functor( "soundResume", soundScriptMethod, &SoundScriptMethod::s_soundResume );
		pybind::def_functor( "soundSourceSetVolume", soundScriptMethod, &SoundScriptMethod::s_soundSourceSetVolume );
		pybind::def_functor( "soundSourceGetVolume", soundScriptMethod, &SoundScriptMethod::s_soundSourceGetVolume );
		pybind::def_functor( "soundSetVolume", soundScriptMethod, &SoundScriptMethod::s_soundSetVolume );
		pybind::def_functor( "soundGetVolume", soundScriptMethod, &SoundScriptMethod::s_soundGetVolume );
		pybind::def_functor( "soundGetPosition", soundScriptMethod, &SoundScriptMethod::s_soundGetPosMs );
		pybind::def_functor( "soundSetPosition", soundScriptMethod, &SoundScriptMethod::s_soundSetPosMs );
		pybind::def_functor_args( "soundFadeIn", soundScriptMethod, &SoundScriptMethod::s_soundFadeIn );
		pybind::def_functor_args( "soundFadeOut", soundScriptMethod, &SoundScriptMethod::s_soundFadeOut );
				
		pybind::def_functor( "musicPlay", soundScriptMethod, &SoundScriptMethod::musicPlay );
		pybind::def_functor( "musicSetVolume", soundScriptMethod, &SoundScriptMethod::musicSetVolume );
		pybind::def_functor( "musicGetVolume", soundScriptMethod, &SoundScriptMethod::musicGetVolume );
		pybind::def_functor( "musicSetVolumeTag", soundScriptMethod, &SoundScriptMethod::musicSetVolumeTag );
		pybind::def_functor( "musicGetVolumeTag", soundScriptMethod, &SoundScriptMethod::musicGetVolumeTag );
		pybind::def_functor( "musicStop", soundScriptMethod, &SoundScriptMethod::musicStop );
        pybind::def_functor( "musicPause", soundScriptMethod, &SoundScriptMethod::musicPause );
        pybind::def_functor( "musicResume", soundScriptMethod, &SoundScriptMethod::musicResume );
		pybind::def_functor( "musicGetDuration", soundScriptMethod, &SoundScriptMethod::s_musicGetDuration );
		pybind::def_functor( "musicGetLengthMs", soundScriptMethod, &SoundScriptMethod::s_musicGetDuration );
		pybind::def_functor( "musicGetPosMs", soundScriptMethod, &SoundScriptMethod::s_musicGetPosMs );
		pybind::def_functor( "musicSetPosMs", soundScriptMethod, &SoundScriptMethod::s_musicSetPosMs );
		pybind::def_functor_args( "musicFadeIn", soundScriptMethod, &SoundScriptMethod::s_musicFadeIn );
		pybind::def_functor( "musicFadeOut", soundScriptMethod, &SoundScriptMethod::s_musicFadeOut );


        pybind::def_functor_args( "voicePlay", soundScriptMethod, &SoundScriptMethod::s_voicePlay );
        pybind::def_functor( "voiceStop", soundScriptMethod, &SoundScriptMethod::s_voiceStop );
		pybind::def_functor( "voicePause", soundScriptMethod, &SoundScriptMethod::s_voicePause );
		pybind::def_functor( "voiceResume", soundScriptMethod, &SoundScriptMethod::s_voiceResume );
        pybind::def_functor( "voiceSetVolume", soundScriptMethod, &SoundScriptMethod::voiceSetVolume );
        pybind::def_functor( "voiceGetVolume", soundScriptMethod, &SoundScriptMethod::voiceGetVolume );
	}
}

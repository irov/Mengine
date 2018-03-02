#	include "PythonScriptWrapper.h"

#	include "Interface/AmplifierInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/PlayerInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Kernel/Affectorable.h"
#	include "Kernel/Affector.h"

#	include "Menge/ResourceSound.h"
#	include "Factory/FactoryPool.h"

#	include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

#	include <math.h>

namespace	Menge
{
	class SoundScriptMethod
	{
    public:
        SoundScriptMethod()
        {
            m_factorySoundAffectorCallback = new FactoryPool<SoundAffectorCallback, 4>();
            m_factoryMusicAffectorCallback = new FactoryPool<MusicAffectorCallback, 4>();
            m_factorySoundNodeListener = new FactoryPool<MySoundNodeListener, 8>();
        }

    public:
		//////////////////////////////////////////////////////////////////////////
		class MySoundNodeListener
			: public SoundListenerInterface
		{
		public:
			MySoundNodeListener()
			{
			}

			~MySoundNodeListener()
			{	
                if( m_resource != nullptr )
                {
                    m_resource->decrementReference();
                    m_resource = nullptr;
                }
			}

		public:
			bool initialize( const ResourceSoundPtr & _resource, const SoundBufferInterfacePtr & _soundBuffer, const pybind::object & _cb, const pybind::args & _args )
			{
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

				if( SOUND_SERVICE()
					->releaseSoundSource( _id ) == false )
				{
					LOGGER_ERROR("MySoundNodeListenerInterface %s emitter invalid release sound %d"
						, m_resource->getName().c_str()
						, _id
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
        typedef stdex::intrusive_ptr<MySoundNodeListener> MySoundNodeListenerPtr;
		//////////////////////////////////////////////////////////////////////////
		bool s_hasSound( const ConstString & _resourceName )
		{
			if( RESOURCE_SERVICE()
				->validResourceType( _resourceName, STRINGIZE_STRING_LOCAL("ResourceSound") ) == false )
			{
				return false;
			}

			return true;
		}
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factorySoundNodeListener;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_createSoundSource( const ConstString & _resourceName, bool _loop, ESoundSourceType _type, const pybind::object & _cb, const pybind::args & _args )
		{
			ResourceSoundPtr resource = RESOURCE_SERVICE()
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

			uint32_t sourceID = SOUND_SERVICE()
                ->createSoundSource( true, soundBuffer, _type, streamable );

			if( sourceID == 0 )
			{
                resource->decrementReference();
                
				return 0;
			}

			SOUND_SERVICE()
				->setLoop( sourceID, _loop );

			float volume = resource->getDefaultVolume();

			if( SOUND_SERVICE()
				->setSourceVolume( sourceID, volume, volume ) == false )
			{                
				LOGGER_ERROR("ScriptWrapper::createSoundSource invalid %s setSourceVolume %f"
					, _resourceName.c_str()
                    , volume
					);

                resource->decrementReference();

                return 0;
			}


            MySoundNodeListenerPtr snlistener = m_factorySoundNodeListener->createObject();

			if( snlistener->initialize( resource, soundBuffer, _cb, _args ) == false )
			{
				LOGGER_ERROR("ScriptWrapper::createSoundSource invalid %s intialize listener"
					, _resourceName.c_str()
					, volume
					);

				resource->decrementReference();

				return 0;
			}

			SOUND_SERVICE()
				->setSourceListener( sourceID, snlistener );
            			
			return sourceID;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_soundPlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb, const pybind::args & _args )
		{
			uint32_t sourceID = s_createSoundSource( _resourceName, _loop, ESST_SOUND, _cb, _args );

			if( sourceID == 0 )
			{
				LOGGER_ERROR("soundPlay: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			if( SOUND_SERVICE()
				->play( sourceID ) == false )
			{
				LOGGER_ERROR("soundPlay: invalid play '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}
              
			return sourceID;
		}
        //////////////////////////////////////////////////////////////////////////
		uint32_t s_voicePlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb, const pybind::args & _args )
        {
            uint32_t sourceID = s_createSoundSource( _resourceName, _loop, ESST_VOICE, _cb, _args );

            if( sourceID == 0 )
            {
                LOGGER_ERROR("voicePlay: can't get resource '%s'"
                    , _resourceName.c_str()
                    );

                return 0;
            }

            if( SOUND_SERVICE()
                ->play( sourceID ) == false )
			{
				LOGGER_ERROR("voicePlay: invalid play '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

            return sourceID;
        }
		//////////////////////////////////////////////////////////////////////////
		bool s_voicePause( uint32_t _soundId )
		{
			bool successful = SOUND_SERVICE()
				->pause( _soundId );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_voiceResume( uint32_t _soundId )
		{
			bool successful = SOUND_SERVICE()
				->resume( _soundId );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_soundPlayFromPosition( const ConstString & _resourceName, float _position, bool _loop, const pybind::object & _cb, const pybind::args & _args )
		{
			uint32_t sourceID = s_createSoundSource( _resourceName, _loop, ESST_SOUND, _cb, _args );

			if( sourceID == 0 )
			{
				LOGGER_ERROR("soundPlayFromPosition: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			if( SOUND_SERVICE()
				->setPosMs( sourceID, _position ) == false )
            {
                LOGGER_ERROR("soundPlayFromPosition: set pos '%s' '%f'"
                    , _resourceName.c_str()
                    , _position
                    );

                return 0;
            }
			
			if( SOUND_SERVICE()
				->play( sourceID ) == false )
            {
                LOGGER_ERROR("soundPlayFromPosition: play '%s' '%f'"
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
			float pos =	SOUND_SERVICE()
				->getPosMs( _sourceID );

			return pos;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundSetPosMs( uint32_t _sourceID, float _pos )
		{
			SOUND_SERVICE()
				->setPosMs( _sourceID, _pos );
		}
		//////////////////////////////////////////////////////////////////////////
		class SoundAffectorCallback
			: public AffectorCallback
		{
		public:
			SoundAffectorCallback()
				: m_sourceId(0)
			{
			}

			~SoundAffectorCallback()
			{
			}

		public:
			void initialize( uint32_t _sourceId, const pybind::object & _cb, const pybind::args & _args )
			{
				m_sourceId = _sourceId;
				m_cb = _cb;
				m_args = _args;
			}

		protected:
			void onAffectorEnd( uint32_t _id, bool _isEnd ) override
			{
				if( _isEnd == true )
				{
					SOUND_SERVICE()
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
			uint32_t m_sourceId;
			pybind::object m_cb;
			pybind::args m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factorySoundAffectorCallback;
		//////////////////////////////////////////////////////////////////////////
		SoundAffectorCallback * createSoundAffectorCallback( uint32_t _sourceId, const pybind::object & _cb, const pybind::args & _args )
		{
			SoundAffectorCallback * callback = m_factorySoundAffectorCallback->createObject();

			callback->initialize( _sourceId, _cb, _args );

			return callback;
		}
		//////////////////////////////////////////////////////////////////////////
		void ___soundFade( uint32_t _sourceId, float _volume )
		{
			SOUND_SERVICE()
				->setSourceMixerVolume( _sourceId, STRINGIZE_STRING_LOCAL( "Fade" ), _volume, _volume );
		}
		//////////////////////////////////////////////////////////////////////////		
		NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<SoundScriptMethod, void (SoundScriptMethod::*)(uint32_t, float), float, uint32_t> m_affectorCreatorSound;
		//////////////////////////////////////////////////////////////////////////
		void s_soundFadeIn( uint32_t _sourceId, float _time, const pybind::object & _cb, const pybind::args & _args )
		{
			SoundAffectorCallback * callback = createSoundAffectorCallback( _sourceId, _cb, _args );

			Affector* affector =
				m_affectorCreatorSound.create( ETA_POSITION
                    , callback, this, &SoundScriptMethod::___soundFade, _sourceId
                    , 1.f, 0.f, _time
                );

			Affectorable * affectorable = PLAYER_SERVICE()
				->getAffectorableGlobal();

			affectorable->addAffector( affector );
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_soundFadeOut( const ConstString & _resourceName, bool _loop, float _time, const pybind::object & _cb, const pybind::args & _args )
		{
			uint32_t sourceId = s_createSoundSource( _resourceName, _loop, ESST_SOUND, _cb, _args );

			if( sourceId == 0 )
			{
				LOGGER_ERROR( "soundPlay: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			if( SOUND_SERVICE()
				->play( sourceId ) == false )
			{
				LOGGER_ERROR( "soundPlay: invalid play '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			Affector* affector =
				m_affectorCreatorSound.create( ETA_POSITION
                    , nullptr, this, &SoundScriptMethod::___soundFade, sourceId
                    , 0.f, 1.f, _time
                );

			Affectorable * affectorable = PLAYER_SERVICE()
				->getAffectorableGlobal();

			affectorable->addAffector( affector );

			return sourceId;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundStop( uint32_t _sourceID )
		{
			SOUND_SERVICE()
				->stop( _sourceID );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundPause( uint32_t _sourceID )
		{
			SOUND_SERVICE()
				->pause( _sourceID );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundResume( uint32_t _sourceID )
		{
			SOUND_SERVICE()
				->resume( _sourceID );
		}
		//////////////////////////////////////////////////////////////////////////
        void s_voiceStop( uint32_t _sourceID )
        {
            SOUND_SERVICE()
                ->stop( _sourceID );
        }
		//////////////////////////////////////////////////////////////////////////
		void s_soundSourceSetVolume( uint32_t _sourceID, float _volume )
		{
			if( SOUND_SERVICE()
				->setSourceVolume( _sourceID, _volume, _volume ) == false )
			{
				LOGGER_ERROR("SoundScriptWrapper::s_soundSourceSetVolume invalid source volume %d"
					, _sourceID
					);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		float s_soundSourceGetVolume( uint32_t _sourceID )
		{
			float volume = SOUND_SERVICE()
				->getSourceVolume( _sourceID );

            return volume;                 
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundSetVolume( float _volume )
		{
			SOUND_SERVICE()
				->setSoundVolume(STRINGIZE_STRING_LOCAL("Generic"), _volume, 0.f );
		}
		//////////////////////////////////////////////////////////////////////////
		float s_soundGetVolume()
		{
			float volume = SOUND_SERVICE()
				->getSoundVolume(STRINGIZE_STRING_LOCAL("Generic") );

            return volume;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_commonSetVolume( float _volume )
		{
			SOUND_SERVICE()
				->setCommonVolume(STRINGIZE_STRING_LOCAL( "Generic" ), _volume, _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float commonGetVolume()
		{
			return SOUND_SERVICE()
				->getCommonVolume(STRINGIZE_STRING_LOCAL("Generic") );
		}
		//////////////////////////////////////////////////////////////////////////
		void musicPlay( const ConstString & _resourceMusic, float _pos, bool _isLooped )
		{
			if( SERVICE_EXIST( AmplifierInterface ) == false )
			{
				return;
			}

			AMPLIFIER_SERVICE()
				->playMusic( _resourceMusic, _pos, _isLooped );
		}
		//////////////////////////////////////////////////////////////////////////
		void musicSetVolume( float _volume )
		{
			SOUND_SERVICE()
				->setMusicVolume(STRINGIZE_STRING_LOCAL("Generic"), _volume, _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float musicGetVolume()
		{
			return SOUND_SERVICE()
				->getMusicVolume(STRINGIZE_STRING_LOCAL("Generic") );
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
				->setVoiceVolume(STRINGIZE_STRING_LOCAL( "Generic" ), _volume, _volume );
        }
        //////////////////////////////////////////////////////////////////////////
        float voiceGetVolume()
        {
            return SOUND_SERVICE()
                ->getVoiceVolume(STRINGIZE_STRING_LOCAL( "Generic") );
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
		float s_musicGetDuration()
		{
			float posMs = AMPLIFIER_SERVICE()
				->getDuration();

			return posMs;
		}
		//////////////////////////////////////////////////////////////////////////
		float s_musicGetPosMs()
		{
			float posMs = AMPLIFIER_SERVICE()
				->getPosMs();

			return posMs;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_musicSetPosMs( float _posMs )
		{
			AMPLIFIER_SERVICE()
				->setPosMs( _posMs );
		}
		//////////////////////////////////////////////////////////////////////////
		void ___musicFade( float _volume )
		{
			SOUND_SERVICE()
				->setMusicVolume(STRINGIZE_STRING_LOCAL( "Fade" ), _volume, _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		class MusicAffectorCallback
			: public AffectorCallback
		{
		public:
			MusicAffectorCallback()
			{
			}

			~MusicAffectorCallback()
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
		FactoryPtr m_factoryMusicAffectorCallback;
		//////////////////////////////////////////////////////////////////////////
		MusicAffectorCallback * createMusicAffectorCallback( const pybind::object & _cb, const pybind::args & _args )
		{
            MusicAffectorCallback * callback = m_factoryMusicAffectorCallback->createObject();

			callback->initialize( _cb, _args );

			return callback; 
		}
		//////////////////////////////////////////////////////////////////////////		
		NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<SoundScriptMethod, void (SoundScriptMethod::*)(float), float> m_affectorCreatorMusic;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_musicFadeIn( float _time, const pybind::object & _cb, const pybind::args & _args )
		{
			if( SERVICE_EXIST( AmplifierInterface ) == false )
			{
				return 0;
			}

			MusicAffectorCallback * callback = createMusicAffectorCallback( _cb, _args );

			Affector* affector = 
				m_affectorCreatorMusic.create( ETA_POSITION
                    , callback, this, &SoundScriptMethod::___musicFade
				, 1.f, 0.f, _time
				);

			Affectorable * affectorable = PLAYER_SERVICE()
				->getAffectorableGlobal();

			AFFECTOR_ID id = affectorable->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_musicFadeOut( const ConstString & _resourceMusic, float _pos, bool _isLooped, float _time )
		{
			if( SERVICE_EXIST( AmplifierInterface ) == false )
			{
				return 0;
			}

			AMPLIFIER_SERVICE()
				->playMusic( _resourceMusic, _pos, _isLooped );

			Affector* affector = 
				m_affectorCreatorMusic.create( ETA_POSITION
                    , nullptr, this, &SoundScriptMethod::___musicFade
				, 0.f, 1.f, _time
				);

			Affectorable * affectorable = PLAYER_SERVICE()
				->getAffectorableGlobal();

			AFFECTOR_ID id = affectorable->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundMute( bool _mute )
		{
			SOUND_SERVICE()
				->mute( _mute );
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_isMute()
		{
			bool mute = SOUND_SERVICE()
				->isMute();

			return mute;
		}
        //////////////////////////////////////////////////////////////////////////
        bool s_isSilent()
        {
            bool silent = SOUND_SYSTEM()
                ->isSilent();

            return silent;
        }
	};

	//////////////////////////////////////////////////////////////////////////
	//REGISTER_SCRIPT_CLASS( Menge, ScriptSoundHelper, Base )
	void PythonScriptWrapper::soundWrap()
	{
        SoundScriptMethod * soundScriptMethod = new SoundScriptMethod();

		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::def_functor( kernel, "hasSound", soundScriptMethod, &SoundScriptMethod::s_hasSound );
		pybind::def_functor( kernel, "isMute", soundScriptMethod, &SoundScriptMethod::s_isMute );
		pybind::def_functor( kernel, "isSilent", soundScriptMethod, &SoundScriptMethod::s_isSilent );
		pybind::def_functor( kernel, "soundMute", soundScriptMethod, &SoundScriptMethod::s_soundMute );

		pybind::def_functor( kernel, "commonSetVolume", soundScriptMethod, &SoundScriptMethod::s_commonSetVolume );
		pybind::def_functor( kernel, "commonGetVolume", soundScriptMethod, &SoundScriptMethod::commonGetVolume );
                
		pybind::def_functor_args( kernel, "soundPlay", soundScriptMethod, &SoundScriptMethod::s_soundPlay );
		pybind::def_functor_args( kernel, "soundPlayFromPosition", soundScriptMethod, &SoundScriptMethod::s_soundPlayFromPosition );
		pybind::def_functor( kernel, "soundStop", soundScriptMethod, &SoundScriptMethod::s_soundStop );
		pybind::def_functor( kernel, "soundPause", soundScriptMethod, &SoundScriptMethod::s_soundPause );
		pybind::def_functor( kernel, "soundResume", soundScriptMethod, &SoundScriptMethod::s_soundResume );
		pybind::def_functor( kernel, "soundSourceSetVolume", soundScriptMethod, &SoundScriptMethod::s_soundSourceSetVolume );
		pybind::def_functor( kernel, "soundSourceGetVolume", soundScriptMethod, &SoundScriptMethod::s_soundSourceGetVolume );
		pybind::def_functor( kernel, "soundSetVolume", soundScriptMethod, &SoundScriptMethod::s_soundSetVolume );
		pybind::def_functor( kernel, "soundGetVolume", soundScriptMethod, &SoundScriptMethod::s_soundGetVolume );
		pybind::def_functor( kernel, "soundGetPosition", soundScriptMethod, &SoundScriptMethod::s_soundGetPosMs );
		pybind::def_functor( kernel, "soundSetPosition", soundScriptMethod, &SoundScriptMethod::s_soundSetPosMs );
		pybind::def_functor_args( kernel, "soundFadeIn", soundScriptMethod, &SoundScriptMethod::s_soundFadeIn );
		pybind::def_functor_args( kernel, "soundFadeOut", soundScriptMethod, &SoundScriptMethod::s_soundFadeOut );
				
		pybind::def_functor( kernel, "musicPlay", soundScriptMethod, &SoundScriptMethod::musicPlay );
		pybind::def_functor( kernel, "musicSetVolume", soundScriptMethod, &SoundScriptMethod::musicSetVolume );
		pybind::def_functor( kernel, "musicGetVolume", soundScriptMethod, &SoundScriptMethod::musicGetVolume );
		pybind::def_functor( kernel, "musicSetVolumeTag", soundScriptMethod, &SoundScriptMethod::musicSetVolumeTag );
		pybind::def_functor( kernel, "musicGetVolumeTag", soundScriptMethod, &SoundScriptMethod::musicGetVolumeTag );
		pybind::def_functor( kernel, "musicStop", soundScriptMethod, &SoundScriptMethod::musicStop );
		pybind::def_functor( kernel, "musicPause", soundScriptMethod, &SoundScriptMethod::musicPause );
		pybind::def_functor( kernel, "musicResume", soundScriptMethod, &SoundScriptMethod::musicResume );
		pybind::def_functor( kernel, "musicGetDuration", soundScriptMethod, &SoundScriptMethod::s_musicGetDuration );
		pybind::def_functor( kernel, "musicGetLengthMs", soundScriptMethod, &SoundScriptMethod::s_musicGetDuration );
		pybind::def_functor( kernel, "musicGetPosMs", soundScriptMethod, &SoundScriptMethod::s_musicGetPosMs );
		pybind::def_functor( kernel, "musicSetPosMs", soundScriptMethod, &SoundScriptMethod::s_musicSetPosMs );
		pybind::def_functor_args( kernel, "musicFadeIn", soundScriptMethod, &SoundScriptMethod::s_musicFadeIn );
		pybind::def_functor( kernel, "musicFadeOut", soundScriptMethod, &SoundScriptMethod::s_musicFadeOut );


		pybind::def_functor_args( kernel, "voicePlay", soundScriptMethod, &SoundScriptMethod::s_voicePlay );
		pybind::def_functor( kernel, "voiceStop", soundScriptMethod, &SoundScriptMethod::s_voiceStop );
		pybind::def_functor( kernel, "voicePause", soundScriptMethod, &SoundScriptMethod::s_voicePause );
		pybind::def_functor( kernel, "voiceResume", soundScriptMethod, &SoundScriptMethod::s_voiceResume );
		pybind::def_functor( kernel, "voiceSetVolume", soundScriptMethod, &SoundScriptMethod::voiceSetVolume );
		pybind::def_functor( kernel, "voiceGetVolume", soundScriptMethod, &SoundScriptMethod::voiceGetVolume );
	}
}

#	include "ScriptWrapper.h"

#	include "Interface/AmplifierInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/PlayerInterface.h"

#	include "SoundEmitter.h"
#	include "ResourceSound.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

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
			bool initialize( ServiceProviderInterface * _serviceProvider, const ResourceSoundPtr & _resource, const SoundBufferInterfacePtr & _soundBuffer, const pybind::object & _cb )
			{
				m_serviceProvider = _serviceProvider;
				m_resource = _resource;
				m_soundBuffer = _soundBuffer;

				m_cb = _cb;				

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
					m_cb( _id );
				}				

				if( SOUND_SERVICE(m_serviceProvider)
					->releaseSoundSource( _id ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("SoundEmitter::_release %s emitter invalid release sound %d"
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
		};
		//////////////////////////////////////////////////////////////////////////
		bool s_hasSound( const ConstString & _resourceName )
		{
			if( RESOURCE_SERVICE(m_serviceProvider)
				->validResourceType( _resourceName, CONST_STRING(m_serviceProvider, ResourceSound) ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_createSoundSource( const ConstString & _resourceName, bool _loop, ESoundSourceType _type, const pybind::object & _cb )
		{
			ResourceSoundPtr resource = RESOURCE_SERVICE(m_serviceProvider)
				->getResourceT<ResourceSoundPtr>( _resourceName );

			if( resource == nullptr )
			{
				return 0;
			}

			SoundBufferInterfacePtr soundBuffer = resource->createSoundBufferNoCache();

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
				->setSourceVolume( sourceID, volume ) == false )
			{                
				LOGGER_ERROR(m_serviceProvider)("ScriptWrapper::createSoundSource invalid %s setSourceVolume %f"
					, _resourceName.c_str()
                    , volume
					);

                resource->decrementReference();

                return 0;
			}


			MySoundNodeListenerInterface * snlistener = new MySoundNodeListenerInterface();

			if( snlistener->initialize( m_serviceProvider, resource, soundBuffer, _cb ) == false )
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
		uint32_t s_soundPlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb )
		{
			uint32_t sourceID = s_createSoundSource( _resourceName, _loop, ESST_SOUND, _cb );

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
		uint32_t s_voicePlay( const ConstString & _resourceName, bool _loop, const pybind::object & _cb )
        {
            uint32_t sourceID = s_createSoundSource( _resourceName, _loop, ESST_VOICE, _cb );

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
		uint32_t s_soundPlayFromPosition( const ConstString & _resourceName, float _position, bool _loop, const pybind::object & _cb )
		{
			uint32_t sourceID = s_createSoundSource(_resourceName, _loop, ESST_SOUND, _cb);

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
		void s_soundStop( uint32_t _sourceID )
		{
			SOUND_SERVICE(m_serviceProvider)
				->stop( _sourceID );
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
				->setSourceVolume( _sourceID, _volume ) == false )
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
				->setSoundVolume( CONST_STRING(m_serviceProvider, Generic), _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float s_soundGetVolume()
		{
			float volume = SOUND_SERVICE(m_serviceProvider)
				->getSoundVolume( CONST_STRING(m_serviceProvider, Generic) );

            return volume;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_commonSetVolume( float _volume )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setCommonVolume( CONST_STRING(m_serviceProvider, Generic), _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float commonGetVolume()
		{
			return SOUND_SERVICE(m_serviceProvider)
				->getCommonVolume( CONST_STRING(m_serviceProvider, Generic) );
		}
		//////////////////////////////////////////////////////////////////////////
		void musicPlayTrack( const ConstString & _list, uint32_t _index, float _pos, bool _isLooped )
		{
			if( SERVICE_EXIST( m_serviceProvider, AmplifierInterface ) == false )
			{
				return;
			}

			AMPLIFIER_SERVICE(m_serviceProvider)
				->playTrack( _list, _index, _pos, _isLooped );
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t musicGetNumTracks()
		{
			if( SERVICE_EXIST( m_serviceProvider, AmplifierInterface ) == false )
			{
				return 0;
			}

			return AMPLIFIER_SERVICE(m_serviceProvider)
				->getNumTracks();
		}
		//////////////////////////////////////////////////////////////////////////
		void musicSetVolume( float _volume )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setMusicVolume( CONST_STRING(m_serviceProvider, Generic), _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float musicGetVolume()
		{
			return SOUND_SERVICE(m_serviceProvider)
				->getMusicVolume( CONST_STRING(m_serviceProvider, Generic) );
		}
        //////////////////////////////////////////////////////////////////////////
        void voiceSetVolume( float _volume )
        {
            SOUND_SERVICE(m_serviceProvider)
                ->setVoiceVolume( CONST_STRING(m_serviceProvider, Generic), _volume );
        }
        //////////////////////////////////////////////////////////////////////////
        float voiceGetVolume()
        {
            return SOUND_SERVICE(m_serviceProvider)
                ->getVoiceVolume( CONST_STRING(m_serviceProvider, Generic) );
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
		bool musicShuffle( const ConstString & _list )
		{
			bool successful = AMPLIFIER_SERVICE(m_serviceProvider)
				->shuffle( _list );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		const ConstString & s_musicGetPlaying()
		{
			return AMPLIFIER_SERVICE(m_serviceProvider)
				->getPlayTrack();
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_musicGetPlayingTrackIndex()
		{
			return AMPLIFIER_SERVICE(m_serviceProvider)
				->getCurrentTrack();
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
				->setMusicVolume( CONST_STRING(m_serviceProvider, Fade), _volume );
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
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb )
			{
				m_serviceProvider = _serviceProvider;
				m_cb = _cb;
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

				m_cb( _id, _isEnd );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPoolStore<MusicAffectorCallback, 4> m_factoryMusicAffectorCallback;
		//////////////////////////////////////////////////////////////////////////
		MusicAffectorCallback * createMusicAffectorCallback( const pybind::object & _cb )
		{
			MusicAffectorCallback * callback = m_factoryMusicAffectorCallback.createObject();

			callback->initialize( m_serviceProvider, _cb );

			return callback; 
		}
		//////////////////////////////////////////////////////////////////////////		
		NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<SoundScriptMethod, void (SoundScriptMethod::*)(float), float> m_affectorCreatorMusic;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_musicFadeIn( float _time, const pybind::object & _cb )
		{
			MusicAffectorCallback * callback = createMusicAffectorCallback( _cb );

			Affector* affector = 
				m_affectorCreatorMusic.create( m_serviceProvider
				, ETA_POSITION, callback, this, &SoundScriptMethod::___musicFade
				, 1.f, 0.f, _time
				, &fabsf
				);

			Affectorable * affectorable = PLAYER_SERVICE(m_serviceProvider)
				->getAffectorableGlobal();

			AFFECTOR_ID id = affectorable->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_musicFadeOut( float _time, const pybind::object & _cb )
		{
			MusicAffectorCallback * callback = createMusicAffectorCallback( _cb );

			Affector* affector = 
				m_affectorCreatorMusic.create( m_serviceProvider
				, ETA_POSITION, callback, this, &SoundScriptMethod::___musicFade
				, 0.f, 1.f, _time
				, &fabsf
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
	void ScriptWrapper::soundWrap( ServiceProviderInterface * _serviceProvider )
	{
        SoundScriptMethod * soundScriptMethod = new SoundScriptMethod(_serviceProvider);

		pybind::def_functor( "hasSound", soundScriptMethod, &SoundScriptMethod::s_hasSound );
        pybind::def_functor( "isMute", soundScriptMethod, &SoundScriptMethod::s_isMute );
        pybind::def_functor( "isSilent", soundScriptMethod, &SoundScriptMethod::s_isSilent );
        pybind::def_functor( "soundMute", soundScriptMethod, &SoundScriptMethod::s_soundMute );

        pybind::def_functor( "commonSetVolume", soundScriptMethod, &SoundScriptMethod::s_commonSetVolume );
        pybind::def_functor( "commonGetVolume", soundScriptMethod, &SoundScriptMethod::commonGetVolume );
                
        pybind::def_functor( "soundPlay", soundScriptMethod, &SoundScriptMethod::s_soundPlay );
		pybind::def_functor( "soundPlayFromPosition", soundScriptMethod, &SoundScriptMethod::s_soundPlayFromPosition );
		pybind::def_functor( "soundStop", soundScriptMethod, &SoundScriptMethod::s_soundStop );
		pybind::def_functor( "soundSourceSetVolume", soundScriptMethod, &SoundScriptMethod::s_soundSourceSetVolume );
		pybind::def_functor( "soundSourceGetVolume", soundScriptMethod, &SoundScriptMethod::s_soundSourceGetVolume );
		pybind::def_functor( "soundSetVolume", soundScriptMethod, &SoundScriptMethod::s_soundSetVolume );
		pybind::def_functor( "soundGetVolume", soundScriptMethod, &SoundScriptMethod::s_soundGetVolume );
		pybind::def_functor( "soundGetPosition", soundScriptMethod, &SoundScriptMethod::s_soundGetPosMs );
		pybind::def_functor( "soundSetPosition", soundScriptMethod, &SoundScriptMethod::s_soundSetPosMs );
				
		pybind::def_functor( "musicPlayTrack", soundScriptMethod, &SoundScriptMethod::musicPlayTrack );
		pybind::def_functor( "musicGetNumTracks", soundScriptMethod, &SoundScriptMethod::musicGetNumTracks );
		pybind::def_functor( "musicSetVolume", soundScriptMethod, &SoundScriptMethod::musicSetVolume );
		pybind::def_functor( "musicGetVolume", soundScriptMethod, &SoundScriptMethod::musicGetVolume );
		pybind::def_functor( "musicStop", soundScriptMethod, &SoundScriptMethod::musicStop );
        pybind::def_functor( "musicPause", soundScriptMethod, &SoundScriptMethod::musicPause );
        pybind::def_functor( "musicResume", soundScriptMethod, &SoundScriptMethod::musicResume );
		pybind::def_functor( "musicShuffle", soundScriptMethod, &SoundScriptMethod::musicShuffle );
		pybind::def_functor( "musicGetPlaying", soundScriptMethod, &SoundScriptMethod::s_musicGetPlaying );
		pybind::def_functor( "musicGetPlayingTrackIndex", soundScriptMethod, &SoundScriptMethod::s_musicGetPlayingTrackIndex );
		pybind::def_functor( "musicGetPosMs", soundScriptMethod, &SoundScriptMethod::s_musicGetPosMs );
		pybind::def_functor( "musicSetPosMs", soundScriptMethod, &SoundScriptMethod::s_musicSetPosMs );
		pybind::def_functor( "musicFadeIn", soundScriptMethod, &SoundScriptMethod::s_musicFadeIn );
		pybind::def_functor( "musicFadeOut", soundScriptMethod, &SoundScriptMethod::s_musicFadeOut );


        pybind::def_functor( "voicePlay", soundScriptMethod, &SoundScriptMethod::s_voicePlay );
        pybind::def_functor( "voiceStop", soundScriptMethod, &SoundScriptMethod::s_voiceStop );
        pybind::def_functor( "voiceSetVolume", soundScriptMethod, &SoundScriptMethod::voiceSetVolume );
        pybind::def_functor( "voiceGetVolume", soundScriptMethod, &SoundScriptMethod::voiceGetVolume );
	}
}

#	include "ScriptWrapper.h"

#	include "Interface/AmplifierServiceInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "SoundEmitter.h"
#	include "ResourceSound.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "pybind/pybind.hpp"

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
			: public SoundNodeListenerInterface
		{
		public:
			MySoundNodeListenerInterface( ServiceProviderInterface * _serviceProvider, ResourceSound * _resource, unsigned int _sourceID, SoundBufferInterface * _soundBuffer, PyObject * _cb )
				: m_serviceProvider(_serviceProvider)
                , m_resource(_resource)
				, m_sourceID(_sourceID)
                , m_soundBuffer(_soundBuffer)
				, m_cb(_cb)
			{
				pybind::incref(m_cb);
			}

			~MySoundNodeListenerInterface()
			{
				pybind::decref(m_cb);
			}

		protected:
			void listenSoundNodePaused() override
			{
				//Empty
			}

			void listenSoundNodeStopped() override
			{				
				if( m_cb != NULL && pybind::is_none( m_cb ) == false )
				{
					pybind::call( m_cb, "(i)", m_sourceID );
				}

				SOUND_SERVICE(m_serviceProvider)
					->releaseSoundSource( m_sourceID );

				if( m_resource != 0 )
				{
                    m_resource->releaseSoundBuffer( m_soundBuffer );

					m_resource->decrementReference();
					m_resource = 0;
				}

				delete this;
			}

		protected:
            ServiceProviderInterface * m_serviceProvider;
            ResourceSound * m_resource;
			unsigned int m_sourceID;	
            SoundBufferInterface * m_soundBuffer;
			PyObject * m_cb;
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
		unsigned int s_createSoundSource( const ConstString & _resourceName, bool _loop, ESoundSourceType _type, PyObject * _cb )
		{
			ResourceSound * resource = RESOURCE_SERVICE(m_serviceProvider)
				->getResourceT<ResourceSound>( _resourceName );

			if( resource == 0 )
			{
				return 0;
			}

			SoundBufferInterface * soundBuffer = resource->createSoundBuffer();

            if( soundBuffer == NULL )
            {
                return 0;
            }

			bool streamable = resource->isStreamable();

			unsigned int sourceID = SOUND_SERVICE(m_serviceProvider)
                ->createSoundSource( true, soundBuffer, _type, streamable );

			if( sourceID == NULL )
			{
				if( resource != NULL )
				{
                    resource->releaseSoundBuffer( soundBuffer );

					resource->decrementReference();
					resource = NULL;
				}				

				return 0;
			}

			SOUND_SERVICE(m_serviceProvider)
				->setLoop( sourceID, _loop );

			float volume = resource->getDefaultVolume();

			if( SOUND_SERVICE(m_serviceProvider)
				->setSourceVolume( sourceID, volume ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptWrapper::createSoundSource invalid  %s"
					, _resourceName.c_str()
					);
			}

			SoundNodeListenerInterface * snlistener = 
				new MySoundNodeListenerInterface( m_serviceProvider, resource, sourceID, soundBuffer, _cb );

			SOUND_SERVICE(m_serviceProvider)
				->setSourceListener( sourceID, snlistener );
			
			return sourceID;
		}
		//////////////////////////////////////////////////////////////////////////
		unsigned int s_soundPlay( const ConstString & _resourceName, bool _loop, PyObject * _cb )
		{
			unsigned int sourceID = s_createSoundSource( _resourceName, _loop, EST_SOUND, _cb );

			if( sourceID == 0 )
			{
				LOGGER_ERROR(m_serviceProvider)( "soundPlay: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			SOUND_SERVICE(m_serviceProvider)
				->play( sourceID );
              
			return sourceID;
		}
        //////////////////////////////////////////////////////////////////////////
        unsigned int s_voicePlay( const ConstString & _resourceName, bool _loop, PyObject * _cb )
        {
            unsigned int sourceID = s_createSoundSource( _resourceName, _loop, EST_VOICE, _cb );

            if( sourceID == 0 )
            {
                LOGGER_ERROR(m_serviceProvider)( "voicePlay: can't get resource '%s'"
                    , _resourceName.c_str()
                    );

                return 0;
            }

            SOUND_SERVICE(m_serviceProvider)
                ->play( sourceID );

            return sourceID;
        }
		//////////////////////////////////////////////////////////////////////////
		unsigned int s_soundPlayFromPosition( const ConstString & _resourceName, float _position, bool _loop, PyObject * _cb )
		{
			unsigned int sourceID = s_createSoundSource(_resourceName, _loop, EST_SOUND, _cb);

			if( sourceID == 0 )
			{
				LOGGER_ERROR(m_serviceProvider)( "soundPlayFromPosition: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			SOUND_SERVICE(m_serviceProvider)
				->setPosMs( sourceID, _position );
			
			SOUND_SERVICE(m_serviceProvider)
				->play( sourceID );

			return sourceID;
		}
		//////////////////////////////////////////////////////////////////////////
		float s_soundGetPosMs( unsigned int _sourceID )
		{
			float pos =	SOUND_SERVICE(m_serviceProvider)
				->getPosMs(_sourceID);

			return pos;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundSetPosMs( unsigned int _sourceID, float _pos )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setPosMs( _sourceID, _pos );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundStop( unsigned int _sourceID )
		{
			SOUND_SERVICE(m_serviceProvider)
				->stop( _sourceID );
		}
        //////////////////////////////////////////////////////////////////////////
        void s_voiceStop( unsigned int _sourceID )
        {
            SOUND_SERVICE(m_serviceProvider)
                ->stop( _sourceID );
        }
		//////////////////////////////////////////////////////////////////////////
		void s_soundSourceSetVolume( unsigned int _sourceID, float _volume )
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
		float s_soundSourceGetVolume( unsigned int _sourceID )
		{
			float volume = SOUND_SERVICE(m_serviceProvider)
				->getSourceVolume( _sourceID );

            return volume;                 
		}
		//////////////////////////////////////////////////////////////////////////
		void s_soundSetVolume( float _volume )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setSoundVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float s_soundGetVolume()
		{
			float volume = SOUND_SERVICE(m_serviceProvider)
				->getSoundVolume();

            return volume;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_commonSetVolume( float _volume )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setCommonVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float commonGetVolume()
		{
			return SOUND_SERVICE(m_serviceProvider)
				->getCommonVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		void musicPlayList( const ConstString & _list )
		{
			AMPLIFIER_SERVICE(m_serviceProvider)
				->playAllTracks( _list );
		}
		//////////////////////////////////////////////////////////////////////////
		void resetPlayList( )
		{
			AMPLIFIER_SERVICE(m_serviceProvider)
				->resetPlayList();
		}
		//////////////////////////////////////////////////////////////////////////
		void musicPlayTrack( const ConstString & _list, int _index, bool _isLooped )
		{
			AMPLIFIER_SERVICE(m_serviceProvider)
				->playTrack( _list, _index, _isLooped );
		}
		//////////////////////////////////////////////////////////////////////////
		size_t musicGetNumTracks()
		{
			return AMPLIFIER_SERVICE(m_serviceProvider)
				->getNumTracks();
		}
		//////////////////////////////////////////////////////////////////////////
		void musicSetVolume( float _volume )
		{
			SOUND_SERVICE(m_serviceProvider)
				->setMusicVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float musicGetVolume()
		{
			return SOUND_SERVICE(m_serviceProvider)
				->getMusicVolume();
		}
        //////////////////////////////////////////////////////////////////////////
        void voiceSetVolume( float _volume )
        {
            SOUND_SERVICE(m_serviceProvider)
                ->setVoiceVolume( _volume );
        }
        //////////////////////////////////////////////////////////////////////////
        float voiceGetVolume()
        {
            return SOUND_SERVICE(m_serviceProvider)
                ->getVoiceVolume();
        }
		//////////////////////////////////////////////////////////////////////////
		void musicStop()
		{
			AMPLIFIER_SERVICE(m_serviceProvider)
				->stop();
		}
		//////////////////////////////////////////////////////////////////////////
		void musicShuffle( const ConstString & _list )
		{
			AMPLIFIER_SERVICE(m_serviceProvider)
				->shuffle( _list );
		}
		//////////////////////////////////////////////////////////////////////////
		const ConstString & s_musicGetPlaying()
		{
			return AMPLIFIER_SERVICE(m_serviceProvider)
				->getPlayTrack();
		}
		//////////////////////////////////////////////////////////////////////////
		void s_musicSetVolume( float _volume )
		{
			AMPLIFIER_SERVICE(m_serviceProvider)
				->setVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		float s_musicGetPosMs()
		{
			return AMPLIFIER_SERVICE(m_serviceProvider)
				->getPosMs();
		}
		//////////////////////////////////////////////////////////////////////////
		void s_musicSetPosMs( float _posMs )
		{
			AMPLIFIER_SERVICE(m_serviceProvider)
				->setPosMs( _posMs );
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

        bool s_isSilent()
        {
            bool silent = SOUND_SERVICE(m_serviceProvider)
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
				
		pybind::def_functor( "musicPlayList", soundScriptMethod, &SoundScriptMethod::musicPlayList );
		pybind::def_functor( "resetPlayList", soundScriptMethod, &SoundScriptMethod::resetPlayList );
		pybind::def_functor( "musicPlayTrack", soundScriptMethod, &SoundScriptMethod::musicPlayTrack );
		pybind::def_functor( "musicGetNumTracks", soundScriptMethod, &SoundScriptMethod::musicGetNumTracks );
		pybind::def_functor( "musicSetVolume", soundScriptMethod, &SoundScriptMethod::musicSetVolume );
		pybind::def_functor( "musicGetVolume", soundScriptMethod, &SoundScriptMethod::musicGetVolume );
		pybind::def_functor( "musicStop", soundScriptMethod, &SoundScriptMethod::musicStop );
		pybind::def_functor( "musicShuffle", soundScriptMethod, &SoundScriptMethod::musicShuffle );
		pybind::def_functor( "musicGetPlaying", soundScriptMethod, &SoundScriptMethod::s_musicGetPlaying );
		pybind::def_functor( "musicSetVolume", soundScriptMethod, &SoundScriptMethod::s_musicSetVolume );
		pybind::def_functor( "musicGetPosMs", soundScriptMethod, &SoundScriptMethod::s_musicGetPosMs );
		pybind::def_functor( "musicSetPosMs", soundScriptMethod, &SoundScriptMethod::s_musicSetPosMs );


        pybind::def_functor( "voicePlay", soundScriptMethod, &SoundScriptMethod::s_voicePlay );
        pybind::def_functor( "voiceStop", soundScriptMethod, &SoundScriptMethod::s_voiceStop );
        pybind::def_functor( "voiceSetVolume", soundScriptMethod, &SoundScriptMethod::voiceSetVolume );
        pybind::def_functor( "voiceGetVolume", soundScriptMethod, &SoundScriptMethod::voiceGetVolume );

	}
}

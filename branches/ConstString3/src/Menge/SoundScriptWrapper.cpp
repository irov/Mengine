#	include "ScriptWrapper.h"

#	include "Amplifier.h"

#	include "SoundEmitter.h"

#	include "ResourceManager.h"
#	include "ResourceSound.h"

#	include "LogEngine.h"

#	include "Core/Holder.h"

#	include "SoundEngine.h"
#	include "Consts.h"

#	include "pybind/pybind.hpp"

namespace	Menge
{
	class ScriptSoundHelper
	{
	public:

		class MySoundNodeListenerInterface
			: public SoundNodeListenerInterface
		{
		public:
			MySoundNodeListenerInterface( ResourceSound * _resource, unsigned int _sourceID, PyObject * _cb )
				: m_resource(_resource)
				, m_sourceID(_sourceID)
				, m_cb(_cb)
			{
				pybind::incref(m_cb);
			}

			~MySoundNodeListenerInterface()
			{
				pybind::decref(m_cb);
			}

		protected:
			void listenPaused() override
			{
				//Empty
			}

			void listenStopped() override
			{				
				if( m_cb != NULL && pybind::is_none( m_cb ) == false )
				{
					pybind::call( m_cb, "(i)", m_sourceID );
				}

				SoundEngine::get()
					->releaseSoundSource( m_sourceID );

				m_resource->decrementReference();

				delete this;
			}

		protected:
			unsigned int m_sourceID;			
			ResourceSound * m_resource;
			PyObject * m_cb;
		};

		static bool s_hasSound( const ConstString & _resourceName )
		{
			if( ResourceManager::get()
				->validResourceType( _resourceName, Consts::get()->c_ResourceSound ) == false )
			{
				return false;
			}

			return true;
		}

		static unsigned int s_soundPlay( const ConstString & _resourceName, bool _loop, PyObject * _cb )
		{
			ResourceSound * resource = ResourceManager::get()
				->getResourceT<ResourceSound>( _resourceName );
			
			if( resource == 0 )
			{
				MENGE_LOG_ERROR( "soundPlay: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			SoundBufferInterface * soundBuffer = resource->get();

			bool streamable = resource->isStreamable();

			unsigned int sourceID = SoundEngine::get()
				->createSoundSource( 
				true
				, soundBuffer
				, streamable
				);

			if( sourceID == 0 )
			{
				resource->decrementReference();

				return 0;
			}

			SoundEngine::get()
				->setLoop( sourceID, _loop );

			float volume = resource->getDefaultVolume();

			SoundEngine::get()
				->setSourceVolume( sourceID, volume );

			SoundNodeListenerInterface * snlistener = 
				new MySoundNodeListenerInterface( resource, sourceID, _cb );

			SoundEngine::get()
				->setSourceListener( sourceID, snlistener );
		
			SoundEngine::get()
				->play( sourceID );

			return sourceID;
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundStop( unsigned int _sourceID )
		{
			SoundEngine::get()
				->stop( _sourceID );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundSourceSetVolume( unsigned int _sourceID, float _volume )
		{
			SoundEngine::get()
				->setSourceVolume( _sourceID, _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float s_soundSourceGetVolume( unsigned int _sourceID )
		{
			return SoundEngine::get()
				->getSourceVolume( _sourceID );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundSetVolume( float _volume )
		{
			SoundEngine::get()
				->setSoundVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float s_soundGetVolume()
		{
			return SoundEngine::get()
				->getSoundVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_commonSetVolume( float _volume )
		{
			SoundEngine::get()
				->setCommonVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float commonGetVolume()
		{
			return SoundEngine::get()
				->getCommonVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicPlayList( const ConstString & _list )
		{
			Amplifier::get()
				->playAllTracks( _list );
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicPlayTrack( const ConstString & _list, int _index, bool _isLooped )
		{
			Amplifier::get()
				->playTrack( _list, _index, _isLooped );
		}
		//////////////////////////////////////////////////////////////////////////
		static size_t musicGetNumTracks()
		{
			return Amplifier::get()
				->getNumTracks();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicSetVolume( float _volume )
		{
			SoundEngine::get()
				->setMusicVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float musicGetVolume()
		{
			return SoundEngine::get()
				->getMusicVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicStop( )
		{
			Amplifier::get()->stop();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicShuffle( const ConstString & _list )
		{
			Amplifier::get()
				->shuffle( _list );
		}
		//////////////////////////////////////////////////////////////////////////
		static const ConstString & s_musicGetPlaying()
		{
			return Amplifier::get()
				->getPlaying();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicVolumeTo( float _time, float _volume )
		{
			Amplifier::get()
				->volumeTo( _time, _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicVolumeToCb( float _time, float _volume, PyObject* _cb )
		{
			Amplifier::get()
				->volumeToCb( _time, _volume, _cb );
		}
		//////////////////////////////////////////////////////////////////////////
		static float s_musicGetPosMs()
		{
			return Amplifier::get()
				->getPosMs();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicSetPosMs( float _posMs )
		{
			Amplifier::get()
				->setPosMs( _posMs );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundMute( bool _mute )
		{
			SoundEngine::get()
				->mute( _mute );
		}
		//////////////////////////////////////////////////////////////////////////
		static bool s_isMute()
		{
			return SoundEngine::get()
				->isMute();
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//REGISTER_SCRIPT_CLASS( Menge, ScriptSoundHelper, Base )
	void ScriptWrapper::soundWrap()
	{
		pybind::def_function( "hasSound", &ScriptSoundHelper::s_hasSound );
		pybind::def_function( "soundPlay", &ScriptSoundHelper::s_soundPlay );
		pybind::def_function( "soundStop", &ScriptSoundHelper::s_soundStop );
		pybind::def_function( "soundSourceSetVolume", &ScriptSoundHelper::s_soundSourceSetVolume );
		pybind::def_function( "soundSourceGetVolume", &ScriptSoundHelper::s_soundSourceGetVolume );
		pybind::def_function( "soundSetVolume", &ScriptSoundHelper::s_soundSetVolume );
		pybind::def_function( "soundGetVolume", &ScriptSoundHelper::s_soundGetVolume );
		pybind::def_function( "soundMute", &ScriptSoundHelper::s_soundMute );
		pybind::def_function( "isMute", &ScriptSoundHelper::s_isMute );
		

		pybind::def_function( "commonSetVolume", &ScriptSoundHelper::s_commonSetVolume );
		pybind::def_function( "commonGetVolume", &ScriptSoundHelper::commonGetVolume );

		pybind::def_function( "musicPlayList", &ScriptSoundHelper::musicPlayList );
		pybind::def_function( "musicPlayTrack", &ScriptSoundHelper::musicPlayTrack );
		pybind::def_function( "musicGetNumTracks", &ScriptSoundHelper::musicGetNumTracks );
		pybind::def_function( "musicSetVolume", &ScriptSoundHelper::musicSetVolume );
		pybind::def_function( "musicGetVolume", &ScriptSoundHelper::musicGetVolume );
		pybind::def_function( "musicStop", &ScriptSoundHelper::musicStop );
		pybind::def_function( "musicShuffle", &ScriptSoundHelper::musicShuffle );
		pybind::def_function( "musicGetPlaying", &ScriptSoundHelper::s_musicGetPlaying );
		pybind::def_function( "musicVolumeTo", &ScriptSoundHelper::s_musicVolumeTo );
		pybind::def_function( "musicVolumeToCb", &ScriptSoundHelper::s_musicVolumeToCb );
		pybind::def_function( "musicGetPosMs", &ScriptSoundHelper::s_musicGetPosMs );
		pybind::def_function( "musicSetPosMs", &ScriptSoundHelper::s_musicSetPosMs );
	}
}

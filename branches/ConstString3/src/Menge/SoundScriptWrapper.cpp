#	include "ScriptWrapper.h"

#	include "Amplifier.h"

#	include "SoundEmitter.h"

#	include "ResourceManager.h"
#	include "ResourceSound.h"

#	include "LogEngine.h"

#	include "Core/Holder.h"

#	include "SoundEngine.h"

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
			MySoundNodeListenerInterface( PyObject * _cb )
				: m_cb(_cb)
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
				pybind::call(m_cb, "()");

				delete this;
			}

		protected:
			PyObject * m_cb;
		};

		static bool soundPlay( const ConstString & _resourceName, PyObject * _cb )
		{
			ResourceSound * resource = ResourceManager::get()
				->getResourceT<ResourceSound>( _resourceName );
			
			if( resource == 0 )
			{
				MENGE_LOG_ERROR( "soundPlay: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return false;
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
				return false;
			}

			SoundEngine::get()
				->setLoop( sourceID, false );

			float volume = resource->getDefaultVolume();

			SoundEngine::get()
				->setVolume( sourceID, volume );

			if( _cb != NULL )
			{
				SoundNodeListenerInterface * snlistener = 
					new MySoundNodeListenerInterface(_cb);

				SoundEngine::get()
					->setSourceListener( sourceID, snlistener );
			}

			SoundEngine::get()
				->play( sourceID );

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		static void soundSetVolume( float _volume )
		{
			SoundEngine::get()
				->setSoundSourceVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float soundGetVolume()
		{
			return SoundEngine::get()
				->getSoundSourceVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void commonSetVolume( float _volume )
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
		static std::size_t musicGetNumTracks()
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
		pybind::def( "soundPlay", &ScriptSoundHelper::soundPlay );
		pybind::def( "soundSetVolume", &ScriptSoundHelper::soundSetVolume );
		pybind::def( "soundGetVolume", &ScriptSoundHelper::soundGetVolume );
		pybind::def( "soundMute", &ScriptSoundHelper::s_soundMute );
		pybind::def( "isMute", &ScriptSoundHelper::s_isMute );
		

		pybind::def( "commonSetVolume", &ScriptSoundHelper::commonSetVolume );
		pybind::def( "commonGetVolume", &ScriptSoundHelper::commonGetVolume );

		pybind::def( "musicPlayList", &ScriptSoundHelper::musicPlayList );
		pybind::def( "musicPlayTrack", &ScriptSoundHelper::musicPlayTrack );
		pybind::def( "musicGetNumTracks", &ScriptSoundHelper::musicGetNumTracks );
		pybind::def( "musicSetVolume", &ScriptSoundHelper::musicSetVolume );
		pybind::def( "musicGetVolume", &ScriptSoundHelper::musicGetVolume );
		pybind::def( "musicStop", &ScriptSoundHelper::musicStop );
		pybind::def( "musicShuffle", &ScriptSoundHelper::musicShuffle );
		pybind::def( "musicGetPlaying", &ScriptSoundHelper::s_musicGetPlaying );
		pybind::def( "musicVolumeTo", &ScriptSoundHelper::s_musicVolumeTo );
		pybind::def( "musicVolumeToCb", &ScriptSoundHelper::s_musicVolumeToCb );
		pybind::def( "musicGetPosMs", &ScriptSoundHelper::s_musicGetPosMs );
		pybind::def( "musicSetPosMs", &ScriptSoundHelper::s_musicSetPosMs );
	}
}

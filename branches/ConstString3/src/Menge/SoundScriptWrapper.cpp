#	include "ScriptWrapper.h"

#	include "Interface/AmplifierServiceInterface.h"

#	include "SoundEmitter.h"

#	include "ResourceManager.h"
#	include "ResourceSound.h"

#	include "LogEngine.h"

#	include "Core/Holder.h"

//#	include "SoundEngine.h"
#	include "Consts.h"

#	include "pybind/pybind.hpp"

namespace	Menge
{
	namespace ScriptSoundHelper
	{
		//////////////////////////////////////////////////////////////////////////
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

				Holder<SoundServiceInterface>::get()
					->releaseSoundSource( m_sourceID );

				if( m_resource != 0 )
				{
					m_resource->decrementReference();
					m_resource = 0;
				}

				delete this;
			}

		protected:
			unsigned int m_sourceID;			
			ResourceSound * m_resource;
			PyObject * m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		static bool s_hasSound( const ConstString & _resourceName )
		{
			if( ResourceManager::get()
				->validResourceType( _resourceName, Consts::get()->c_ResourceSound ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		static unsigned int s_createSoundSource( const ConstString & _resourceName, bool _loop, PyObject * _cb )
		{
			ResourceSound * resource = ResourceManager::get()
				->getResourceT<ResourceSound>( _resourceName );

			if( resource == 0 )
			{
				return 0;
			}

			SoundBufferInterface * soundBuffer = resource->getSoundBuffer();

			bool streamable = resource->isStreamable();

			unsigned int sourceID = Holder<SoundServiceInterface>::get()
				->createSoundSource( 
				true
				, soundBuffer
				, streamable
				);

			if( sourceID == 0 )
			{
				if( resource != 0 )
				{
					resource->decrementReference();
					resource = 0;
				}				

				return 0;
			}

			Holder<SoundServiceInterface>::get()
				->setLoop( sourceID, _loop );

			float volume = resource->getDefaultVolume();

			if( Holder<SoundServiceInterface>::get()
				->setSourceVolume( sourceID, volume ) == false )
			{
				MENGE_LOG_ERROR("ScriptWrapper::createSoundSource invalid  %s"
					, _resourceName.c_str()
					);
			}

			SoundNodeListenerInterface * snlistener = 
				new MySoundNodeListenerInterface( resource, sourceID, _cb );

			Holder<SoundServiceInterface>::get()
				->setSourceListener( sourceID, snlistener );
			
			return sourceID;
		}
		//////////////////////////////////////////////////////////////////////////
		static unsigned int s_soundPlay( const ConstString & _resourceName, bool _loop, PyObject * _cb )
		{
			unsigned int sourceID = s_createSoundSource(_resourceName, _loop, _cb);

			if( sourceID == 0 )
			{
				MENGE_LOG_ERROR( "soundPlay: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			Holder<SoundServiceInterface>::get()
				->play( sourceID );

			return sourceID;
		}
		//////////////////////////////////////////////////////////////////////////
		static unsigned int s_soundPlayFromPosition( const ConstString & _resourceName, float _position, bool _loop, PyObject * _cb )
		{
			unsigned int sourceID = s_createSoundSource(_resourceName, _loop, _cb);
			if( sourceID == 0 )
			{
				MENGE_LOG_ERROR( "soundPlayFromPosition: can't get resource '%s'"
					, _resourceName.c_str()
					);

				return 0;
			}

			Holder<SoundServiceInterface>::get()
				->setPosMs( sourceID, _position );
			
			Holder<SoundServiceInterface>::get()
				->play( sourceID );

			return sourceID;
		}
		//////////////////////////////////////////////////////////////////////////
		static float s_soundGetPosMs( unsigned int _sourceID )
		{
			float pos =	Holder<SoundServiceInterface>::get()
				->getPosMs(_sourceID);

			return pos;
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundSetPosMs( unsigned int _sourceID, float _pos )
		{
			Holder<SoundServiceInterface>::get()
				->setPosMs( _sourceID, _pos );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundStop( unsigned int _sourceID )
		{
			Holder<SoundServiceInterface>::get()
				->stop( _sourceID );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundSourceSetVolume( unsigned int _sourceID, float _volume )
		{
			if( Holder<SoundServiceInterface>::get()
				->setSourceVolume( _sourceID, _volume ) == false )
			{
				MENGE_LOG_ERROR("SoundScriptWrapper::s_soundSourceSetVolume invalid source volume %d"
					, _sourceID
					);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		static float s_soundSourceGetVolume( unsigned int _sourceID )
		{
			return Holder<SoundServiceInterface>::get()
				->getSourceVolume( _sourceID );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundSetVolume( float _volume )
		{
			Holder<SoundServiceInterface>::get()
				->setSoundVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float s_soundGetVolume()
		{
			return Holder<SoundServiceInterface>::get()
				->getSoundVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_commonSetVolume( float _volume )
		{
			Holder<SoundServiceInterface>::get()
				->setCommonVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float commonGetVolume()
		{
			return Holder<SoundServiceInterface>::get()
				->getCommonVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicPlayList( const ConstString & _list )
		{
			Holder<AmplifierServiceInterface>::get()
				->playAllTracks( _list );
		}
		//////////////////////////////////////////////////////////////////////////
		static void resetPlayList( )
		{
			Holder<AmplifierServiceInterface>::get()
				->resetPlayList();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicPlayTrack( const ConstString & _list, int _index, bool _isLooped )
		{
			Holder<AmplifierServiceInterface>::get()
				->playTrack( _list, _index, _isLooped );
		}
		//////////////////////////////////////////////////////////////////////////
		static size_t musicGetNumTracks()
		{
			return Holder<AmplifierServiceInterface>::get()
				->getNumTracks();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicSetVolume( float _volume )
		{
			Holder<SoundServiceInterface>::get()
				->setMusicVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float musicGetVolume()
		{
			return Holder<SoundServiceInterface>::get()
				->getMusicVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicStop()
		{
			Holder<AmplifierServiceInterface>::get()
				->stop();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicShuffle( const ConstString & _list )
		{
			Holder<AmplifierServiceInterface>::get()
				->shuffle( _list );
		}
		//////////////////////////////////////////////////////////////////////////
		static const ConstString & s_musicGetPlaying()
		{
			return Holder<AmplifierServiceInterface>::get()
				->getPlayTrack();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicSetVolume( float _volume )
		{
			Holder<AmplifierServiceInterface>::get()
				->setVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float s_musicGetPosMs()
		{
			return Holder<AmplifierServiceInterface>::get()
				->getPosMs();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicSetPosMs( float _posMs )
		{
			Holder<AmplifierServiceInterface>::get()
				->setPosMs( _posMs );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundMute( bool _mute )
		{
			Holder<SoundServiceInterface>::get()
				->mute( _mute );
		}
		//////////////////////////////////////////////////////////////////////////
		static bool s_isMute()
		{
			bool mute = Holder<SoundServiceInterface>::get()
				->isMute();

			return mute;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//REGISTER_SCRIPT_CLASS( Menge, ScriptSoundHelper, Base )
	void ScriptWrapper::soundWrap()
	{
		pybind::def_function( "hasSound", &ScriptSoundHelper::s_hasSound );
		pybind::def_function( "soundPlay", &ScriptSoundHelper::s_soundPlay );
		pybind::def_function( "soundPlayFromPosition", &ScriptSoundHelper::s_soundPlayFromPosition );
		pybind::def_function( "soundStop", &ScriptSoundHelper::s_soundStop );
		pybind::def_function( "soundSourceSetVolume", &ScriptSoundHelper::s_soundSourceSetVolume );
		pybind::def_function( "soundSourceGetVolume", &ScriptSoundHelper::s_soundSourceGetVolume );
		pybind::def_function( "soundSetVolume", &ScriptSoundHelper::s_soundSetVolume );
		pybind::def_function( "soundGetVolume", &ScriptSoundHelper::s_soundGetVolume );
		pybind::def_function( "soundGetPosition", &ScriptSoundHelper::s_soundGetPosMs );
		pybind::def_function( "soundSetPosition", &ScriptSoundHelper::s_soundSetPosMs );
		pybind::def_function( "soundMute", &ScriptSoundHelper::s_soundMute );
		pybind::def_function( "isMute", &ScriptSoundHelper::s_isMute );
		

		pybind::def_function( "commonSetVolume", &ScriptSoundHelper::s_commonSetVolume );
		pybind::def_function( "commonGetVolume", &ScriptSoundHelper::commonGetVolume );

		pybind::def_function( "musicPlayList", &ScriptSoundHelper::musicPlayList );
		pybind::def_function( "resetPlayList", &ScriptSoundHelper::resetPlayList );
		pybind::def_function( "musicPlayTrack", &ScriptSoundHelper::musicPlayTrack );
		pybind::def_function( "musicGetNumTracks", &ScriptSoundHelper::musicGetNumTracks );
		pybind::def_function( "musicSetVolume", &ScriptSoundHelper::musicSetVolume );
		pybind::def_function( "musicGetVolume", &ScriptSoundHelper::musicGetVolume );
		pybind::def_function( "musicStop", &ScriptSoundHelper::musicStop );
		pybind::def_function( "musicShuffle", &ScriptSoundHelper::musicShuffle );
		pybind::def_function( "musicGetPlaying", &ScriptSoundHelper::s_musicGetPlaying );
		pybind::def_function( "musicSetVolume", &ScriptSoundHelper::s_musicSetVolume );
		pybind::def_function( "musicGetPosMs", &ScriptSoundHelper::s_musicGetPosMs );
		pybind::def_function( "musicSetPosMs", &ScriptSoundHelper::s_musicSetPosMs );
	}
}

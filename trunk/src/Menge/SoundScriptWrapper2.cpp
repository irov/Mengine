//#	include "ScriptWrapper.h"
//
//#	include <xxbind/xxbind.hpp>
//
//#	include "Amplifier.h"
//
//#	include "SoundEmitter.h"
//
//#	include "ResourceManager.h"
//#	include "ResourceSound.h"
//
//#	include "LogEngine.h"
//
//#	include "Core/Holder.h"
//
//#	include "SoundEngine.h"
//#	include "Consts.h"
//
//namespace	Menge
//{
//	namespace ScriptSoundHelper
//	{
//		//////////////////////////////////////////////////////////////////////////
//		class MySoundNodeListenerInterface
//			: public SoundNodeListenerInterface
//		{
//		public:
//			MySoundNodeListenerInterface( ResourceSound * _resource, unsigned int _sourceID, const xxbind::object & _cb )
//				: m_resource(_resource)
//				, m_sourceID(_sourceID)				
//				, m_cb(_cb)
//			{
//			}
//
//			~MySoundNodeListenerInterface()
//			{
//			}
//
//		protected:
//			void listenSoundNodePaused() override
//			{
//				//Empty
//			}
//
//			void listenSoundNodeStopped() override
//			{				
//				if( m_cb.is_none() == false )
//				{
//					m_cb.call( "(i)", m_sourceID );
//				}
//
//				SoundEngine::get()
//					->releaseSoundSource( m_sourceID );
//
//				if( m_resource != 0 )
//				{
//					m_resource->decrementReference();
//					m_resource = 0;
//				}
//
//				delete this;
//			}
//
//		protected:
//			unsigned int m_sourceID;			
//			ResourceSound * m_resource;			
//			xxbind::object m_cb;
//		};
//		//////////////////////////////////////////////////////////////////////////
//		static bool s_hasSound( const ConstString & _resourceName )
//		{
//			if( ResourceManager::get()
//				->validResourceType( _resourceName, Consts::get()->c_ResourceSound ) == false )
//			{
//				return false;
//			}
//
//			return true;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static unsigned int s_createSoundSource( const ConstString & _resourceName, bool _loop, const xxbind::object & _cb )
//		{
//			ResourceSound * resource = ResourceManager::get()
//				->getResourceT<ResourceSound>( _resourceName );
//
//			if( resource == 0 )
//			{
//				return 0;
//			}
//
//			SoundBufferInterface * soundBuffer = resource->getSoundBuffer();
//
//			bool streamable = resource->isStreamable();
//
//			unsigned int sourceID = SoundEngine::get()
//				->createSoundSource( 
//				true
//				, soundBuffer
//				, streamable
//				);
//
//			if( sourceID == 0 )
//			{
//				if( resource != 0 )
//				{
//					resource->decrementReference();
//					resource = 0;
//				}				
//
//				return 0;
//			}
//
//			SoundEngine::get()
//				->setLoop( sourceID, _loop );
//
//			float volume = resource->getDefaultVolume();
//
//			if( SoundEngine::get()
//				->setSourceVolume( sourceID, volume ) == false )
//			{
//				MENGE_LOG_ERROR("ScriptWrapper::createSoundSource invalid  %s"
//					, _resourceName.c_str()
//					);
//			}
//
//			SoundNodeListenerInterface * snlistener = 
//				new MySoundNodeListenerInterface(resource, sourceID, _cb);
//
//			SoundEngine::get()
//				->setSourceListener( sourceID, snlistener );
//			
//			return sourceID;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static unsigned int s_soundPlay( const ConstString & _resourceName, bool _loop, const xxbind::object & _cb )
//		{
//			unsigned int sourceID = s_createSoundSource( _resourceName, _loop, _cb );
//
//			if( sourceID == 0 )
//			{
//				MENGE_LOG_ERROR( "soundPlay: can't get resource '%s'"
//					, _resourceName.c_str()
//					);
//
//				return 0;
//			}
//
//			SoundEngine::get()
//				->play( sourceID );
//
//			return sourceID;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static unsigned int s_soundPlayFromPosition( const ConstString & _resourceName, float _position, bool _loop, const xxbind::object & _cb )
//		{
//			unsigned int sourceID = s_createSoundSource( _resourceName, _loop, _cb );
//			if( sourceID == 0 )
//			{
//				MENGE_LOG_ERROR( "soundPlayFromPosition: can't get resource '%s'"
//					, _resourceName.c_str()
//					);
//
//				return 0;
//			}
//
//			SoundEngine::get()
//				->setPosMs( sourceID, _position );
//			
//			SoundEngine::get()
//				->play( sourceID );
//
//			return sourceID;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static float s_soundGetPosMs( unsigned int _sourceID )
//		{
//			float pos =	SoundEngine::get()
//				->getPosMs(_sourceID);
//
//			return pos;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void s_soundSetPosMs( unsigned int _sourceID, float _pos )
//		{
//			SoundEngine::get()
//				->setPosMs( _sourceID, _pos );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void s_soundStop( unsigned int _sourceID )
//		{
//			SoundEngine::get()
//				->stop( _sourceID );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void s_soundSourceSetVolume( unsigned int _sourceID, float _volume )
//		{
//			if( SoundEngine::get()
//				->setSourceVolume( _sourceID, _volume ) == false )
//			{
//				MENGE_LOG_ERROR("SoundScriptWrapper::s_soundSourceSetVolume invalid source volume %d"
//					, _sourceID
//					);
//			}
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static float s_soundSourceGetVolume( unsigned int _sourceID )
//		{
//			return SoundEngine::get()
//				->getSourceVolume( _sourceID );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void s_soundSetVolume( float _volume )
//		{
//			SoundEngine::get()
//				->setSoundVolume( _volume );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static float s_soundGetVolume()
//		{
//			return SoundEngine::get()
//				->getSoundVolume();
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void s_commonSetVolume( float _volume )
//		{
//			SoundEngine::get()
//				->setCommonVolume( _volume );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static float commonGetVolume()
//		{
//			return SoundEngine::get()
//				->getCommonVolume();
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void musicPlayList( const ConstString & _list )
//		{
//			Amplifier::get()
//				->playAllTracks( _list );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void musicPlayTrack( const ConstString & _list, int _index, bool _isLooped )
//		{
//			Amplifier::get()
//				->playTrack( _list, _index, _isLooped );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static size_t musicGetNumTracks()
//		{
//			return Amplifier::get()
//				->getNumTracks();
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void musicSetVolume( float _volume )
//		{
//			SoundEngine::get()
//				->setMusicVolume( _volume );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static float musicGetVolume()
//		{
//			return SoundEngine::get()
//				->getMusicVolume();
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void musicStop()
//		{
//			Amplifier::get()->stop();
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void musicShuffle( const ConstString & _list )
//		{
//			Amplifier::get()
//				->shuffle( _list );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static const ConstString & s_musicGetPlaying()
//		{
//			return Amplifier::get()
//				->getPlaying();
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void s_musicVolumeTo( float _time, float _volume )
//		{
//			Amplifier::get()
//				->volumeTo( _time, _volume );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void s_musicVolumeToCb( float _time, float _volume, PyObject* _cb )
//		{
//			Amplifier::get()
//				->volumeToCb( _time, _volume, _cb );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static float s_musicGetPosMs()
//		{
//			return Amplifier::get()
//				->getPosMs();
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void s_musicSetPosMs( float _posMs )
//		{
//			Amplifier::get()
//				->setPosMs( _posMs );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static void s_soundMute( bool _mute )
//		{
//			SoundEngine::get()
//				->mute( _mute );
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static bool s_isMute()
//		{
//			return SoundEngine::get()
//				->isMute();
//		}
//	};
//
//	//////////////////////////////////////////////////////////////////////////
//	//REGISTER_SCRIPT_CLASS( Menge, ScriptSoundHelper, Base )
//	void soundWrap( xxbind::facade_script * _facade, xxbind::module * _module )
//	{
//		xxbind::def_function( _facade, _module, "hasSound", &ScriptSoundHelper::s_hasSound );
//		xxbind::def_function( _facade, _module, "soundPlay", &ScriptSoundHelper::s_soundPlay );
//		xxbind::def_function( _facade, _module, "soundPlayFromPosition", &ScriptSoundHelper::s_soundPlayFromPosition );
//		xxbind::def_function( _facade, _module, "soundStop", &ScriptSoundHelper::s_soundStop );
//		xxbind::def_function( _facade, _module, "soundSourceSetVolume", &ScriptSoundHelper::s_soundSourceSetVolume );
//		xxbind::def_function( _facade, _module, "soundSourceGetVolume", &ScriptSoundHelper::s_soundSourceGetVolume );
//		xxbind::def_function( _facade, _module, "soundSetVolume", &ScriptSoundHelper::s_soundSetVolume );
//		xxbind::def_function( _facade, _module, "soundGetVolume", &ScriptSoundHelper::s_soundGetVolume );
//		xxbind::def_function( _facade, _module, "soundGetPosition", &ScriptSoundHelper::s_soundGetPosMs );
//		xxbind::def_function( _facade, _module, "soundSetPosition", &ScriptSoundHelper::s_soundSetPosMs );
//		xxbind::def_function( _facade, _module, "soundMute", &ScriptSoundHelper::s_soundMute );
//		xxbind::def_function( _facade, _module, "isMute", &ScriptSoundHelper::s_isMute );
//		
//
//		xxbind::def_function( _facade, _module, "commonSetVolume", &ScriptSoundHelper::s_commonSetVolume );
//		xxbind::def_function( _facade, _module, "commonGetVolume", &ScriptSoundHelper::commonGetVolume );
//
//		xxbind::def_function( _facade, _module, "musicPlayList", &ScriptSoundHelper::musicPlayList );
//		xxbind::def_function( _facade, _module, "musicPlayTrack", &ScriptSoundHelper::musicPlayTrack );
//		xxbind::def_function( _facade, _module, "musicGetNumTracks", &ScriptSoundHelper::musicGetNumTracks );
//		xxbind::def_function( _facade, _module, "musicSetVolume", &ScriptSoundHelper::musicSetVolume );
//		xxbind::def_function( _facade, _module, "musicGetVolume", &ScriptSoundHelper::musicGetVolume );
//		xxbind::def_function( _facade, _module, "musicStop", &ScriptSoundHelper::musicStop );
//		xxbind::def_function( _facade, _module, "musicShuffle", &ScriptSoundHelper::musicShuffle );
//		xxbind::def_function( _facade, _module, "musicGetPlaying", &ScriptSoundHelper::s_musicGetPlaying );
//		xxbind::def_function( _facade, _module, "musicVolumeTo", &ScriptSoundHelper::s_musicVolumeTo );
//		xxbind::def_function( _facade, _module, "musicVolumeToCb", &ScriptSoundHelper::s_musicVolumeToCb );
//		xxbind::def_function( _facade, _module, "musicGetPosMs", &ScriptSoundHelper::s_musicGetPosMs );
//		xxbind::def_function( _facade, _module, "musicSetPosMs", &ScriptSoundHelper::s_musicSetPosMs );
//	}
//}

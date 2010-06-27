#	include "ScriptWrapper.h"

#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "Amplifier.h"

#	include "SoundEmitter.h"

#	include "Core/Holder.h"

#	include "SoundEngine.h"

#	include "pybind/pybind.hpp"

namespace	Menge
{
	class ScriptSoundHelper
	{
	public:
		static void soundSetVolume( float _volume )
		{
			SoundEngine::get()->setSoundSourceVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float soundGetVolume()
		{
			return SoundEngine::get()->getSoundSourceVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void commonSetVolume( float _volume )
		{
			SoundEngine::get()->setCommonVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float commonGetVolume()
		{
			return SoundEngine::get()->getCommonVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicPlayList( const String& _list )
		{
			ConstString clist = ConstManager::get()
				->genString( _list );

			Amplifier::get()
				->playAllTracks( clist );
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicPlayTrack( const String& _list, int _index, bool _isLooped )
		{
			ConstString clist = ConstManager::get()
				->genString( _list );

			Amplifier::get()
				->playTrack( clist, _index, _isLooped );
		}
		//////////////////////////////////////////////////////////////////////////
		static std::size_t musicGetNumTracks()
		{
			return Amplifier::get()->getNumTracks();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicSetVolume( float _volume )
		{
			SoundEngine::get()->setMusicVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float musicGetVolume()
		{
			return SoundEngine::get()->getMusicVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicStop( )
		{
			Amplifier::get()->stop();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicShuffle( const String& _list )
		{
			ConstString clist = ConstManager::get()
				->genString( _list );

			Amplifier::get()
				->shuffle( clist );
		}
		//////////////////////////////////////////////////////////////////////////
		static const String& s_musicGetPlaying()
		{
			const ConstString & list = Amplifier::get()->getPlaying();

			return list.str();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicVolumeTo( float _time, float _volume )
		{
			Amplifier::get()->volumeTo( _time, _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicVolumeToCb( float _time, float _volume, PyObject* _cb )
		{
			Amplifier::get()->volumeToCb( _time, _volume, _cb );
		}
		//////////////////////////////////////////////////////////////////////////
		static float s_musicGetPosMs()
		{
			return Amplifier::get()->getPosMs();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicSetPosMs( float _posMs )
		{
			Amplifier::get()->setPosMs( _posMs );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundMute( bool _mute )
		{
			SoundEngine::get()->mute( _mute );
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//REGISTER_SCRIPT_CLASS( Menge, ScriptSoundHelper, Base )
	void ScriptWrapper::soundWrap()
	{
		pybind::def( "soundSetVolume", &ScriptSoundHelper::soundSetVolume );
		pybind::def( "soundGetVolume", &ScriptSoundHelper::soundGetVolume );
		pybind::def( "soundMute", &ScriptSoundHelper::s_soundMute );

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

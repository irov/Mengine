#	include "ScriptWrapper.h"

#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "Amplifier.h"

#	include "SoundEmitter.h"

#	include "Holder.h"

#	include "SoundEngine.h"

#	include "pybind/pybind.hpp"

namespace	Menge
{
	class ScriptSoundHelper
	{
	public:
		static void soundSetVolume( float _volume )
		{
			Holder<SoundEngine>::hostage()->setSoundSourceVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float soundGetVolume()
		{
			return Holder<SoundEngine>::hostage()->getSoundSourceVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void commonSetVolume( float _volume )
		{
			Holder<SoundEngine>::hostage()->setCommonVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float commonGetVolume()
		{
			return Holder<SoundEngine>::hostage()->getCommonVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicPlayList( const String& _list )
		{
			Holder<Amplifier>::hostage()->playAllTracks( _list );
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicPlayTrack( const String& _list, int _index, bool _isLooped )
		{
			Holder<Amplifier>::hostage()->playTrack(_list, _index, _isLooped);
		}
		//////////////////////////////////////////////////////////////////////////
		static std::size_t musicGetNumTracks()
		{
			return Holder<Amplifier>::hostage()->getNumTracks();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicSetVolume( float _volume )
		{
			Holder<SoundEngine>::hostage()->setMusicVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float musicGetVolume()
		{
			return Holder<SoundEngine>::hostage()->getMusicVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicStop( )
		{
			Holder<Amplifier>::hostage()->stop();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicShuffle( const String& _list )
		{
			Holder<Amplifier>::hostage()->shuffle( _list );
		}
		//////////////////////////////////////////////////////////////////////////
		static const String& s_musicGetPlaying()
		{
			return Holder<Amplifier>::hostage()->getPlaying();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicVolumeTo( float _time, float _volume )
		{
			Holder<Amplifier>::hostage()->volumeTo( _time, _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicVolumeToCb( float _time, float _volume, PyObject* _cb )
		{
			Holder<Amplifier>::hostage()->volumeToCb( _time, _volume, _cb );
		}
		//////////////////////////////////////////////////////////////////////////
		static float s_musicGetPosMs()
		{
			return Holder<Amplifier>::hostage()->getPosMs();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicSetPosMs( float _posMs )
		{
			Holder<Amplifier>::hostage()->setPosMs( _posMs );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_soundMute( bool _mute )
		{
			Holder<SoundEngine>::hostage()->mute( _mute );
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

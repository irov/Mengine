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
		static void musicPlayList( const std::string & _list )
		{
			Holder<Amplifier>::hostage()->playAllTracks( _list );
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicPlayTrack( const std::string & _list, int _index, bool _isLooped )
		{
			Holder<Amplifier>::hostage()->playTrack(_list, _index, _isLooped);
		}
		//////////////////////////////////////////////////////////////////////////
		static int musicGetNumTracks()
		{
			return Holder<Amplifier>::hostage()->getNumTracks();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicSetVolume( float _volume )
		{
			Holder<Amplifier>::hostage()->setVolume( _volume );
		}
		//////////////////////////////////////////////////////////////////////////
		static float musicGetVolume()
		{
			return Holder<Amplifier>::hostage()->getVolume();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicStop( )
		{
			Holder<Amplifier>::hostage()->stop();
		}
		//////////////////////////////////////////////////////////////////////////
		static void musicShuffle( const std::string & _list )
		{
			Holder<Amplifier>::hostage()->shuffle( _list );
		}
		//////////////////////////////////////////////////////////////////////////
		static const String& s_musicGetPlaying()
		{
			return Holder<Amplifier>::hostage()->getPlaying();
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicFadeIn( float _time )
		{
			Holder<Amplifier>::hostage()->fadeIn( _time );
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_musicFadeOut( float _time )
		{
			Holder<Amplifier>::hostage()->fadeOut( _time );
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//REGISTER_SCRIPT_CLASS( Menge, ScriptSoundHelper, Base )
	void ScriptWrapper::soundWrap()
	{
		pybind::def( "soundSetVolume", &ScriptSoundHelper::soundSetVolume );
		pybind::def( "soundGetVolume", &ScriptSoundHelper::soundGetVolume );

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
		pybind::def( "musicFadeIn", &ScriptSoundHelper::s_musicFadeIn );
		pybind::def( "musicFadeOut", &ScriptSoundHelper::s_musicFadeOut );
	}
}
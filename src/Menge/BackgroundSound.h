#	include <vector>

#	include "SoundEngine.h"

#	include "SoundNode.h"

#	include "PlayList.h"

#	include "SoundSystemInterface.h"

namespace	Menge
{
	class	BackgroundSound	: public SoundNodeListenerInterface
	{
	public:
		BackgroundSound();
		void	play(Playlist& _playList);
		void	update();
		bool	listenRecycled(SoundSourceInterface*	_sn);
		void	listenStoped(SoundSourceInterface*	_sn);
		void	listenEnded(SoundSourceInterface*	_sn);
	private:
		enum	FADE_STATES
		{
			FADE_DOWN,
			FADE_UP,
			NO_FADE,
		};
		SoundNode		m_playingTrack;
		std::string		m_currentNamePlayList;
		Playlist* 		m_currentPlayList;
		FADE_STATES		m_fadeState;
		std::string		m_currentSoundTrackName;
		void	_beginFade();
	};
};
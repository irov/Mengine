#	 pragma once

#	include <map>

#	include "time.h"

#	include "SoundSystemInterface.h"

class	SoundBufferInterface;
class	SoundSourceInterface;
class	SoundNodeListenerInterface;

namespace	Menge
{
	class Playlist;

	typedef	std::map<std::string,Playlist*>	TPlayListMap;

	class Amplifier
		:	public SoundNodeListenerInterface
	{
		public:
			Amplifier();
			~Amplifier();
			void	stop();
			void	playList(const std::string& _playListName);
			void	loadPlayList(const std::string& _filename);
			void	addPlayList(const std::string& _playListFileName);
			void	erasePlayList(const std::string& _playListFileName);
			void	update(float _timing);
		private:
			TPlayListMap	m_playLists;
			Playlist *	m_currentPlayList;
			bool	m_fadeState;
			bool	m_isPaused;
			bool	m_isMusicDead;
			
			SoundSourceInterface * m_music;
			SoundBufferInterface * m_sampleMusic;

			clock_t	m_timeFadeBegin;
			clock_t	m_timeFadeEnd;
			float	m_fadeTime;
			float	m_fadeoffTime;

			void	beginFade();
			void	updateFadeParams(SoundSourceInterface* sound);
			void	listenPaused(bool _pause);
			void	listenStopped();
			void	releaseMusic(bool _dead);
	};
}
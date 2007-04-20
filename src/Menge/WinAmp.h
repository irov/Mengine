#	 pragma once

#	include <map>

#	include "time.h"

#	include "SoundSystemInterface.h"

class	FileDataInterface;
class	SoundSourceInterface;
class	SoundNodeListenerInterface;

namespace	Menge
{
	class Playlist;

	typedef	std::map<std::string,Playlist*>	TPlayListMap;

	class WinAmp
		:	public SoundNodeListenerInterface
	{
		public:
			WinAmp();
			~WinAmp();
			void	setFadeTime(double _fadeTime);
			void	playList(const std::string& _playListName);
			void	loadPlayList(const std::string& _filename);
			void	addPlayList(const std::string& _playListFileName);
			void	erasePlayList(const std::string& _playListFileName);
			void	update(double _timing);
			void	listenPaused(bool _pause);
			void	listenStopped();
		private:
			void	_beginFade();
		private:
			TPlayListMap	m_playLists;
			Playlist*	m_currentPlayList;
			bool	m_fadeState;
			bool	m_changeTrack;
			bool	m_isPaused;

			SoundSourceInterface* m_soundSource;
			FileDataInterface* m_fileData;

			clock_t	m_timeFadeBegin;
			clock_t	m_timeFadeEnd;
			double	m_fadeTime;
			double	m_fadeoffTime;
	};
};
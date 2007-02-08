#	 pragma once

#	include <map>

#	include "time.h"

class	FileDataInterface;
class	SoundSourceInterface;

namespace	Menge
{
	class Playlist;

	class BackgroundSound;

	typedef	std::map<std::string,Playlist*>	TPlayListMap;

	class BacksoundManager
	{
		public:
			BacksoundManager();
			~BacksoundManager();
			void	setFadeTime(double _fadeTime);
			void	playList(const std::string& _playListName);
			void	loadPlayList(const std::string& _filename);
			void	addPlayList(const std::string& _playListFileName);
			void	erasePlayList(const std::string& _playListFileName);
			void	update(double _timing);
		private:
			void	_beginFade();
		private:
			TPlayListMap	m_playLists;
			Playlist*	m_currentPlayList;
			bool	m_fadeState;

			SoundSourceInterface* m_soundSource;
			FileDataInterface* m_fileData;

			clock_t	m_timeFadeBegin;
			clock_t	m_timeFadeEnd;
			double	m_fadeTime;
			double	m_fadeoffTime;
	};
};
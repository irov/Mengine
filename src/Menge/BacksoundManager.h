#	 pragma once

#	include <string>
#	include <map>

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
			void	playList(const std::string& _playListName);
			void	loadPlayList(const std::string& _filename);
			void	addPlayList(const std::string& _playListFileName);
			void	erasePlayList(const std::string& _playListFileName);
			void	update();
		private:
			BackgroundSound	* m_backSound;
			TPlayListMap	m_playLists;
	};
};
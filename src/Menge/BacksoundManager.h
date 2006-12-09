#	 pragma once

#	include <string>

#	include <map>

#	include "SoundSystemInterface.h"

#	include "FileEngine.h"

namespace	Menge
{
	class Playlist;

	class BackgroundSound;

	typedef	std::map<std::string,Playlist*>	TPlayListMap;	

	enum	FADE_STATES
	{
		FADE_DOWN,
		FADE_UP,
		NO_FADE,
	};

	class BacksoundManager : public SoundNodeListenerInterface
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
			TPlayListMap	m_playLists;
			bool	listenRecycled(SoundSourceInterface*	_sn);
			void	listenStoped(SoundSourceInterface*	_sn);
			void	listenEnded(SoundSourceInterface*	_sn);
		private:
			float m_fadeVelocity;
			std::string	m_currentNamePlayList;
			FADE_STATES	m_fadeState;
			Playlist * m_currentPlayList;
		private:
			SoundSourceInterface* m_soundSource;
			FileDataInterface* m_fileData;	
			void	_beginFade();
	};
};
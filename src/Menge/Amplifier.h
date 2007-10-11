#	 pragma once

#	include <map>

#	include "time.h"

#	include "Interface/SoundSystemInterface.h"

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
			void	update( size_t _timing );
		private:
			TPlayListMap	m_playLists;
			Playlist *	m_currentPlayList;
			bool	m_fadeState;
			bool	m_isPaused;
			bool	m_isMusicDead;
			
			SoundSourceInterface * m_music;
			SoundBufferInterface * m_sampleMusic;

			float	m_fadeTime;

			void	updateFadeParams(SoundSourceInterface* _sound);
			void	listenPaused(bool _pause);
			void	listenStopped();
			void	releaseMusic(bool _dead);
			void	setVolume(float _newVolume);
	};
}
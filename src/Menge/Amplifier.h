#	 pragma once

#	include <map>

#	include "time.h"

#	include <vector>

#	include "Interface/SoundSystemInterface.h"

/* #	define FADE_PARAM */

namespace	Menge
{
	class ResourcePlaylist;
	class Playlist;

	typedef	std::map<std::string,Playlist*>	TPlayListMap;

	class Amplifier
		:	public SoundNodeListenerInterface
	{
		public:
			Amplifier();
			~Amplifier();
			void	stop();
			void	setVolume(float _newVolume);
			void	playList( const std::string& _playListName );
			void	update( size_t _timing );
		private:
			ResourcePlaylist * m_playlistResource;

			TPlayListMap	m_playLists;
			Playlist *	m_currentPlayList;
			bool	m_fadeState;
			bool	m_isPaused;
			bool	m_isMusicDead;
			
			SoundSourceInterface * m_music;
			SoundBufferInterface * m_sampleMusic;

			float	m_fadeTime;

			void	addPlayList( const std::string& _playListFileName, const std::vector<std::string> & _tracks );
			void	updateFadeParams( SoundSourceInterface * _sound );
			void	listenPaused( bool _pause );
			void	listenStopped();
			void	releaseMusic( bool _dead );	
	};
}
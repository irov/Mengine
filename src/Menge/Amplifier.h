#	 pragma once

#	include "Interface/SoundSystemInterface.h"

#	include <map>

namespace	Menge
{
	class ResourcePlaylist;
	class Playlist;

	class Amplifier
		:	public SoundNodeListenerInterface
	{
		public:
			Amplifier();
			~Amplifier();
			void	stop();
			void	setVolume( float _vol );
			float	getVolume() const;
			void	playList( const std::string& _playListName );
			void	update( size_t _timing );
		private:
			ResourcePlaylist * m_playlistResource;

			bool m_changeTrack;

			float m_volume;

			typedef	std::map<std::string,Playlist*>	TPlayListMap;

			TPlayListMap	m_playLists;
			Playlist *	m_currentPlayList;
			
			SoundSourceInterface * m_music;
			SoundBufferInterface * m_sampleMusic;

			void	listenPaused( bool _pause );
			void	listenStopped();
			void	release();	
			void	initStream();
	};
}
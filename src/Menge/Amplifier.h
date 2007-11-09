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
		public:
			void	play( const std::string & _playlist );
			void	update( float _timing );
			void	stop();

			void	setVolume( float _volume );
			float	getVolume() const;

		public:	

		//	void	registerPlaylist( Playlist * _playlist );
		//	void	unregisterPlaylist( Playlist * _playlist );

		private:
			bool m_changeTrack;

			float m_volume;

			typedef	std::map<std::string,Playlist*>	TPlayListMap;

			TPlayListMap	m_playLists;

			Playlist *	m_currentPlayList;
			
			SoundSourceInterface * m_music;
			SoundBufferInterface * m_buffer;

			void	listenPaused( bool _pause );
			void	listenStopped();
			void	release_();	
			void	play_();
	};
}
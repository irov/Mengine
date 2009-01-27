#	 pragma once

#	include "Config/Typedef.h"

#	include "Interface/SoundSystemInterface.h"
#	include "ValueInterpolator.h"
#	include <map>

namespace	Menge
{
	class ResourcePlaylist;
	class Playlist;

	//! Amplifier - проигрывает музыку. 
	/*! 
	*
	*/

	class Amplifier
		:	public SoundNodeListenerInterface
	{
		public:
			//! Конструктор. Изначально громкость 100%.
			/*!
			*/
			Amplifier();

			//! Деструктор.
			/*!
			*/
			~Amplifier();

			//! Проигрывание плейлиста. Плейлист задается как ресурс.
			/*!
			\param _playlist имя ресурса.
			*/
			void	playAllTracks( const String& _playlistResource );

			//! Shuffle плейлиста.
			/*!
			\param _playlist имя плейлиста.
			*/
			void	shuffle( const String& _playlist );

			//! Остановка музыки.
			/*!
			*/
			void	stop();

			const String& getPlaying() const;

			void volumeTo( float _time, float _value );
			std::size_t getNumTracks() const;

			void update( float _timing );

			void playTrack( const String& _playlistResource, int _index, bool _looped );

			float getPosMs();
			void setPosMs( float _posMs );

		private:
			float m_volume;
			float m_volumeOverride;

			typedef	std::map< String, Playlist* >	TMapPlayList;

			TMapPlayList	m_mapPlayLists;

			String	m_currentPlaylistName;
			Playlist *	m_currentPlayList;
			
			unsigned int m_sourceID;
			SoundBufferInterface * m_buffer;

			void	listenPaused();
			void	listenStopped();
			void	_release();	
			void	_prepareSound( const String& _filename );
			bool	_loadPlayList( const String& _playlistResource );

			ValueInterpolator<float> m_volumeTo;
			bool m_playing;
			bool m_needRefocus;
	};
}
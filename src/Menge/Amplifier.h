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
			void	playAllTracks( const std::string & _playlistResource );

			//! Shuffle плейлиста.
			/*!
			\param _playlist имя плейлиста.
			*/
			void	shuffle( const std::string & _playlist );

			//! Остановка музыки.
			/*!
			*/
			void	stop();

			//! Установка громкости. Задается от [0;1]
			/*!
			\param _volume значение громкости.
			*/

			void	setVolume( float _volume );

			//! Возвращает величину громкости
			/*!
			\return величина громкости
			*/
			float	getVolume() const;

			const String& getPlaying() const;

			void fadeIn( float _time );
			void fadeOut( float _time );
			int getNumTracks() const;

			void update( float _timing );

			void playTrack( const std::string & _playlistResource, int _index, bool _looped );

			
		private:
			float m_volume;
			float m_volumeOverride;

			typedef	std::map< std::string, Playlist* >	TMapPlayList;

			TMapPlayList	m_mapPlayLists;

			String	m_currentPlaylistName;
			Playlist *	m_currentPlayList;
			
			SoundSourceInterface * m_music;
			SoundBufferInterface * m_buffer;

			void	listenPaused( bool _pause );
			void	listenStopped();
			void	_release();	
			void	_prepareSound(const std::string & _filename);
			bool	_loadPlayList(const std::string & _playlistResource);

			ValueInterpolator<float> m_fade;
	};
}
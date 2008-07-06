#	 pragma once

#	include "Interface/SoundSystemInterface.h"

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
			void	play( const std::string & _playlist );

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

		private:
			float m_volume;

			typedef	std::map< std::string, Playlist* >	TMapPlayList;

			TMapPlayList	m_mapPlayLists;

			Playlist *	m_currentPlayList;
			
			SoundSourceInterface * m_music;
			SoundBufferInterface * m_buffer;

			void	listenPaused( bool _pause );
			void	listenStopped();
			void	_release();	
			void	_play();
	};
}
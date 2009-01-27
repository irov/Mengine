#	pragma once

#	include "Config/Typedef.h"

#	include <vector>

namespace	Menge
{
	class ResourcePlaylist;

	//! Playlist - список имен треков. Использует ресурс ResourcePlaylist для загрузки имен треков и определения зацикленности.
	/*! 
	* 
	*/

	class Playlist
	{
	public:
		//! Конструктор. Изначально можно задать сразу ресурс. Иначе через setPlaylistResource.
		/*!
		\param _resource ресурс.
		*/
		Playlist( ResourcePlaylist * _resource = NULL );

		//! Деструктор.
		/*!
		*/
		~Playlist();
	public:
		//! Установка зацикленности.
		/*!
		\param _loop флаг зацикливания.
		*/
		void setLooped( bool _loop );

		//! Возвращает флаг зацикливания
		/*!
		\return флаг зацикливания
		*/
		bool getLooped() const;

		//! Возвращает признак окончания проигрывания всех треков.
		/*!
		*/
		bool isEnded() const;

		//! Выбрать следующий трек.
		/*!
		*/
		void next();
		
		//! Выбрать предыдущий трек.
		/*!
		*/
		void previos();
		
		//! Выбрать первый трек.
		/*!
		*/
		void first();

		//! Выбрать последний трек.
		/*!
		*/
		void last();

		//! Случайная равномерная расстановка треков. После расстановки устанавливается первый трек.
		/*!
		*/
		void shuffle();
		
		//! Возвращает имя текущего трека. Если текущий трек не установлен, то вернет пустую строку.
		/*!
		\return имя текущего трека
		*/
		String getTrack() const;

		//! Установка плейлист ресурса. 
		/*!
		\param _resource ресурс.
		*/
		bool setPlaylistResource( ResourcePlaylist * _resource );

		//! Возвращает количество треков.
		/*!
		
		*/
		std::size_t	numTracks() const;

		//! Возвращает трек.
		/*!


		*/
		String getTrackByIndex( std::size_t _index );

		void setTrack(std::size_t _index);
		void setLooped1( bool _loop );

	private:
		ResourcePlaylist * m_playlistResource;

		typedef	std::vector<String> TVecTrack;

		TVecTrack m_tracks;
		TVecTrack::iterator	m_track;
		
		bool m_loop;

		bool m_oneTrackPlayed;
		bool m_oneTrackLooped;
		String m_category;
	};

};

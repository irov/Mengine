#	pragma once

#	include "Config/Typedef.h"

#	include <vector>

namespace	Menge
{
	class ResourcePlaylist;

	//! Playlist - ������ ���� ������. ���������� ������ ResourcePlaylist ��� �������� ���� ������ � ����������� �������������.
	/*! 
	* 
	*/

	class Playlist
	{
	public:
		//! �����������. ���������� ����� ������ ����� ������. ����� ����� setPlaylistResource.
		/*!
		\param _resource ������.
		*/
		Playlist( ResourcePlaylist * _resource = NULL );

		//! ����������.
		/*!
		*/
		~Playlist();
	public:
		//! ��������� �������������.
		/*!
		\param _loop ���� ������������.
		*/
		void setLooped( bool _loop );

		//! ���������� ���� ������������
		/*!
		\return ���� ������������
		*/
		bool getLooped() const;

		//! ���������� ������� ��������� ������������ ���� ������.
		/*!
		*/
		bool isEnded() const;

		//! ������� ��������� ����.
		/*!
		*/
		void next();
		
		//! ������� ���������� ����.
		/*!
		*/
		void previous();
		
		//! ������� ������ ����.
		/*!
		*/
		void first();

		//! ������� ��������� ����.
		/*!
		*/
		void last();

		//! ��������� ����������� ����������� ������. ����� ����������� ��������������� ������ ����.
		/*!
		*/
		void shuffle();
		
		//! ���������� ��� �������� �����. ���� ������� ���� �� ����������, �� ������ ������ ������.
		/*!
		\return ��� �������� �����
		*/
		String getTrack() const;

		//! ��������� �������� �������. 
		/*!
		\param _resource ������.
		*/
		bool setPlaylistResource( ResourcePlaylist * _resource );

		//! ���������� ���������� ������.
		/*!
		
		*/
		std::size_t	numTracks() const;

		//! ���������� ����.
		/*!


		*/
		String getTrackByIndex( std::size_t _index );

		void setTrack(std::size_t _index);
		void setLooped1( bool _loop );

		const String& getCategory() const;
	private:
		ResourcePlaylist * m_playlistResource;

		TVectorString m_tracks;
		TVectorString::iterator	m_track;
		
		bool m_loop;

		bool m_oneTrackPlayed;
		bool m_oneTrackLooped;
		String m_category;
	};

};

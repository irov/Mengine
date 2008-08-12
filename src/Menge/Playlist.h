#	pragma once

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
		void previos();
		
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
		const std::string &	getTrack() const;

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
		const std::string & getTrackByIndex( std::size_t _index );

		void setTrack(const std::string & _name);
		void setLooped1( bool _loop );

	private:
		ResourcePlaylist * m_playlistResource;

		typedef	std::vector<std::string> TVecTrack;

		TVecTrack m_tracks;
		TVecTrack::iterator	m_track;

		bool m_loop;

		bool m_oneTrackPlayed;
		bool m_oneTrackLooped;
	};

};
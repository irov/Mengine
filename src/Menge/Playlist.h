#	pragma once

#	include <vector>

namespace	Menge
{
	typedef	std::vector<std::string> TVecTrack;

	class TiXmlElement;

	class ResourcePlaylist;

	class Playlist
	{
	public:
		Playlist( ResourcePlaylist * _resource );
		~Playlist();
	public:
		void setLooped( bool _loop );
		bool getLooped() const;

		bool isEnded() const;

		void next();
		void previos();

		void first();
		void last();

		void shuffle();

		const std::string &	getTrackName() const;
		void addTrack( const std::string & _track );

		bool setPlaylistResource( ResourcePlaylist * _resource );

	private:
		bool m_loop;

		ResourcePlaylist * m_playlistResource;

		TVecTrack m_tracks;
		TVecTrack::iterator	m_currentSong;
	};

};
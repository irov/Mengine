#	pragma once

#	include <vector>

namespace	Menge
{
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

		const std::string &	getCurrentTrack() const;

	public:
		bool setPlaylistResource( ResourcePlaylist * _resource );

	private:
		ResourcePlaylist * m_playlistResource;

		typedef	std::vector<std::string> TVecTrack;

		TVecTrack m_tracks;
		TVecTrack::iterator	m_track;

		bool m_loop;
	};

};
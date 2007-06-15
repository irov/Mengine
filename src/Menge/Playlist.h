#	pragma once

#	include <vector>

class TiXmlElement;

namespace	Menge
{
	typedef	std::vector<std::string> TListTrackVec;

	class Playlist
	{
	public:
		Playlist(const std::string& _playlistName);
		~Playlist();
	public:
		void loadTracks();
		void shuffle();
		void nextSong();
		void prevSong();
		void firstSong();
		void lastSong();
		void clear();
		const std::string&	getCurrentSongName() const;
		const std::string&	getName() const;
		bool isLoaded() const;
	private:
		bool m_isTracksLoaded;
		std::string	m_playListName;
		TListTrackVec m_tracks;
		TListTrackVec::iterator	m_currentSong;
	};

};
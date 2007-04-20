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
		/* Хранение имени плейлиста возможно избыточно, потому что оно используется менеджером
		BacksoundManager в std::map<std::string,Playlist>
		*/
		std::string	m_playListName;
		TListTrackVec m_tracks;
		TListTrackVec::iterator	m_currentSong;
	};

};
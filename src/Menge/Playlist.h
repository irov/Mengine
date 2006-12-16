#	pragma once

#	include <list>

class TiXmlElement;

namespace	Menge
{
	typedef	std::list<std::string> TListTrackList;

	class Playlist
	{
	public:
		Playlist(const std::string& _playlistName);
		~Playlist();

	public:
		void loadTracks();

	public:
		void nextSong();
		void release();
		const std::string&	getCurrentSongName() const;
		const std::string&	getName() const;
		bool isLoaded() const;
	private:
		bool m_isTracksLoaded;
		std::string	m_playListName;
		TListTrackList m_tracks;
		TListTrackList::iterator	m_currentSoundTrack;
	};

};
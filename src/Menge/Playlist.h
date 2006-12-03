#	pragma once

#	include <vector>

class TiXmlElement;

namespace	Menge
{
	typedef	std::vector<std::string> TVecTrackList;

	class Playlist
	{
	public:
		Playlist(const std::string& name);

	public:
		void	nextSong();
		void	loader(TiXmlElement * xml);
		void	release();
		const std::string&	getCurrentSongName() const;
		const std::string&	getName() const;

	private:
		std::string	m_playListName;
		TVecTrackList m_tracks;
		TVecTrackList::iterator	m_currentSoundTrack;
	};

};
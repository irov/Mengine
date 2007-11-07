#	pragma once

#	include <vector>

class TiXmlElement;

namespace	Menge
{
	typedef	std::vector<std::string> TVecTrack;

	class Playlist
	{
	public:
		Playlist( const std::string& _playlistName );
		~Playlist();
	public:
		void setLooped( bool _loop );
		bool getLooped() const;
		bool isEnded() const;
		void shuffle();
		void nextSong();
		void prevSong();
		void firstSong();
		void lastSong();
		void clear();
		const std::string&	getCurrentSongName() const;
		const std::string&	getName() const;
		void addTrack( const std::string & _track );
	private:
		std::string	m_playListName;
		TVecTrack m_tracks;
		TVecTrack::iterator	m_currentSong;
		bool m_loop;
	};

};
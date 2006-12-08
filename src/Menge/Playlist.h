#	pragma once

#	include <vector>

#	include "NodeImpl.h"

class TiXmlElement;

namespace	Menge
{
	typedef	std::vector<std::string> TVecTrackList;

	class Playlist : public NodeImpl
	{
		OBJECT_DECLARE(Playlist)
	public:
		Playlist();
		~Playlist();
	public:
		void loader( TiXmlElement *_xml ) override;
	public:
		void	nextSong();
		void	release();
		const std::string&	getCurrentSongName() const;
		const std::string&	getName() const;

	private:
		std::string	m_playListName;
		TVecTrackList m_tracks;
		TVecTrackList::iterator	m_currentSoundTrack;
	};

};
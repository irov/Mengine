#include "Playlist.h"

#	include "XmlParser.h"

namespace	Menge
{
	Playlist::Playlist(const std::string& name)
	{
		m_playListName = name;
		//////////////////////////////////////////////////////////////////////////
		m_tracks.push_back("1.wav");
		m_tracks.push_back("2.wav");
		//m_tracks.push_back("3.wav");
		m_currentSoundTrack = m_tracks.begin();
		//////////////////////////////////////////////////////////////////////////
	}
	void	Playlist::nextSong()
	{
		++m_currentSoundTrack;
		if (m_currentSoundTrack == m_tracks.end())
		{
			m_currentSoundTrack	= m_tracks.begin();
		}
	}

	const std::string&	Playlist::getCurrentSongName() const
	{
		return	*m_currentSoundTrack;
	}

	const std::string&	Playlist::getName() const
	{
		return	m_playListName;
	}

	void	Playlist::loader(TiXmlElement * xml)
	{
		std::string	m_filename;

		XML_FOR_EACH_TREE(xml)
		{
			XML_CHECK_VALUE_NODE("Filename","Value",m_filename);
			m_tracks.push_back(m_filename);
		}

		m_currentSoundTrack = m_tracks.begin();
	}

	void	Playlist::release()
	{
		m_tracks.clear();
	}
};
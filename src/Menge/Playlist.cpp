#	include "ObjectImplement.h"

#	include "Playlist.h"

#	include "XmlParser.h"

OBJECT_IMPLEMENT( Playlist )

Playlist::Playlist()
{
}

Playlist::~Playlist()
{
	int u = 0;
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
		XML_CHECK_NODE_FOR_EACH("Tracks")
		{
			XML_CHECK_NODE("Track")
			{
				XML_VALUE_ATTRIBUTE("Value",m_filename);
				m_tracks.push_back(m_filename);
			}
		}
	}

	NodeImpl::loader(xml);

	m_currentSoundTrack = m_tracks.begin();
}

void	Playlist::release()
{
	m_tracks.clear();
}
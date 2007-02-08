#	include "Playlist.h"

#	include "XmlParser.h"

#	include "FileEngine.h"

using namespace Menge;

Playlist::Playlist(const std::string& _playlistName)
	: m_playListName(_playlistName)
	, m_isTracksLoaded(false)
{
}

Playlist::~Playlist()
{
}

void	Playlist::nextSong()
{
	if (++m_currentSoundTrack == m_tracks.end())
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

void	Playlist::loadTracks()
{
	if(isLoaded() == true)
	{
		return;
	}

	std::string	filename;

	XML_PARSE_FILE_EX(m_playListName)
	{
		XML_CHECK_NODE_FOR_EACH("Tracks")
		{
			XML_CHECK_NODE("Track")
			{
				XML_VALUE_ATTRIBUTE("File",filename);
				m_tracks.push_back(filename);
			}
		}
	}

	m_isTracksLoaded = true;

	m_currentSoundTrack = m_tracks.begin();
}

bool	Playlist::isLoaded() const
{
	return m_isTracksLoaded;
}

void	Playlist::release()
{
	m_tracks.clear();
}
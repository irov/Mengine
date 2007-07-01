#	include "Playlist.h"

#	include "XmlParser.h"

#	include "FileEngine.h"

using namespace Menge;
//////////////////////////////////////////////////////////////////////////
Playlist::Playlist(const std::string& _playlistName)
	: m_playListName(_playlistName)
	, m_isTracksLoaded(false)
{
}
//////////////////////////////////////////////////////////////////////////
Playlist::~Playlist()
{
}
//////////////////////////////////////////////////////////////////////////
void	Playlist::shuffle()
{
	for(int i = 0; i < m_tracks.size(); ++i) 
	{
		int rnd = rand() % m_tracks.size();
		std::swap(m_tracks[i],m_tracks[rnd]);
	}
}
//////////////////////////////////////////////////////////////////////////
void	Playlist::nextSong()
{
	if (++m_currentSong == m_tracks.end())
	{
		m_currentSong = m_tracks.begin();
	}
}
//////////////////////////////////////////////////////////////////////////
void	Playlist::prevSong()
{
	if(m_currentSong == m_tracks.begin())
	{
		m_currentSong = m_tracks.end();
	}
	--m_currentSong;
}
//////////////////////////////////////////////////////////////////////////
void	Playlist::firstSong()
{
	m_currentSong = m_tracks.begin();
}
//////////////////////////////////////////////////////////////////////////
void	Playlist::lastSong()
{
	m_currentSong = m_tracks.end() - 1;
}
//////////////////////////////////////////////////////////////////////////
const std::string&	Playlist::getCurrentSongName() const
{
	return	*m_currentSong;
}
//////////////////////////////////////////////////////////////////////////
const std::string&	Playlist::getName() const
{
	return	m_playListName;
}
//////////////////////////////////////////////////////////////////////////
void	Playlist::loadTracks()
{
	if(isLoaded() == true)
	{
		firstSong();
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

	firstSong();
}
//////////////////////////////////////////////////////////////////////////
bool	Playlist::isLoaded() const
{
	return m_isTracksLoaded;
}
//////////////////////////////////////////////////////////////////////////
void	Playlist::clear()
{
	m_tracks.clear();
}
//////////////////////////////////////////////////////////////////////////
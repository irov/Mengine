#	include "BacksoundManager.h"
#	include "BackgroundSound.h"
#	include "Playlist.h"

#	include "XmlParser.h"

#	include "FileEngine.h"


namespace	Menge
{
	BacksoundManager::BacksoundManager()
	: m_backSound(0)
	{
		m_backSound = new BackgroundSound();
	}

	BacksoundManager::~BacksoundManager()
	{
		for (TPlayListMap::iterator it = m_playLists.begin(); it!=m_playLists.end(); ++it)
		{
			delete (*it).second;
		}
		delete m_backSound;
	}

	void	BacksoundManager::playList(const std::string& _playListName)
	{
		TPlayListMap::iterator it = m_playLists.find(_playListName);

		if (it == m_playLists.end())
		{
			assert(!"PlayList not found!");
			return;
		}

		Playlist * playlist = (*it).second;

		playlist->loadTracks();
	
		m_backSound->play(*playlist);
	}

	void	BacksoundManager::loadPlayList(const std::string& _filename)
	{
		std::string playListFileName;

		XML_PARSE_FILE_EX(_filename)
		{
			XML_CHECK_NODE_FOR_EACH("Pls")
			{
				XML_CHECK_VALUE_NODE("Pl","File",playListFileName)
				{
					addPlayList(playListFileName);
				}
			}
		}
	}

	void	BacksoundManager::addPlayList(const std::string& _playListFileName)
	{
		m_playLists.insert(
			std::make_pair(_playListFileName, new Playlist(_playListFileName))
			);
	}

	void	BacksoundManager::erasePlayList(const std::string& _playListFileName)
	{
		m_playLists.erase(_playListFileName);
	}

	void	BacksoundManager::update()
	{
		m_backSound->update();
	}
};
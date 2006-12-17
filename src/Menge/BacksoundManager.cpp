#	include "BacksoundManager.h"

#	include "Playlist.h"

#	include "XmlParser.h"

#	include "FileEngine.h"

#	include "SoundEngine.h"

namespace	Menge
{
	BacksoundManager::BacksoundManager()
	: m_soundSource(0)
	, m_fadeVelocity(0.05f)
	, m_fileData(0)
	, m_currentPlayList(0)
	, m_fadeState(NO_FADE)
	{
	}

	BacksoundManager::~BacksoundManager()
	{
		for (TPlayListMap::iterator it = m_playLists.begin(); it!=m_playLists.end(); ++it)
		{
			delete (*it).second;
		}
		
		if(m_soundSource)
		{
			Keeper<SoundEngine>::hostage()->deleteSound(m_soundSource);
		}

		if (m_fileData)
		{
			Keeper<FileEngine>::hostage()->closeFile(m_fileData);
		}
	}

	void	BacksoundManager::_beginFade()
	{
		m_fadeState = FADE_UP;
		Keeper<SoundEngine>::hostage()->addSoundNode(
			m_soundSource,
			m_fileData,
			m_currentPlayList->getCurrentSongName(),
			this, true
			);
		m_soundSource->setVolume(0);
		m_soundSource->play();
	}

	void	BacksoundManager::playList(const std::string& _playListName)
	{
		TPlayListMap::iterator it = m_playLists.find(_playListName);

		if (it == m_playLists.end())
		{
			assert(!"PlayList not found!");
			return;
		}

		m_currentPlayList = (*it).second;

		m_currentPlayList->loadTracks();
	
		static bool firstInit = true;

		if (firstInit)
		{
			_beginFade();
			firstInit = false;
			return;
		}

		if (m_soundSource->isPlaying() && !firstInit)
		{
			m_fadeState = FADE_DOWN;
		}
		else
		{
			_beginFade();
		}
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

	bool	BacksoundManager::listenRecycled(SoundSourceInterface* _sn)
	{
		printf("listenRecycled \n");
		return true;
	}

	void	BacksoundManager::listenStoped(SoundSourceInterface*	_sn)
	{
		printf("listenStoped \n");
	} 

	void	BacksoundManager::listenEnded(SoundSourceInterface* _sn)
	{
		m_currentPlayList->nextSong();

		Keeper<SoundEngine>::hostage()->deleteSound(m_soundSource);
		m_soundSource = NULL;

		Keeper<FileEngine>::hostage()->closeFile(m_fileData);
		m_fileData = NULL;

		_beginFade();

		printf("listenEnded \n");
	}

	void	BacksoundManager::update()
	{
		m_soundSource->updateSoundBuffer();

		switch(m_fadeState)
		{
		case FADE_UP:
			{
				printf("FADE UP \n");
				float newVolume = m_soundSource->getVolume() + m_fadeVelocity;
				m_soundSource->setVolume(newVolume);

				if (m_soundSource->getVolume() >= 100.0f)
				{
					m_soundSource->setVolume(100.0f);
					m_fadeState = NO_FADE;
				}
			}
			break;

		case FADE_DOWN:
			{
				printf("FADE_DOWN \n");
				float newVolume = m_soundSource->getVolume() - m_fadeVelocity;
				m_soundSource->setVolume(newVolume);

				if (m_soundSource->getVolume() <= 0)
				{
					m_soundSource->stop();
				}
			}
			break;

		case NO_FADE:
			{
				printf("NO_FADE \n");

				double totalSize = m_soundSource->getTotalSize();
				double currPos = m_soundSource->getPos();

				if (currPos >= totalSize * 0.05)	
					//	if ((totalSize - currPos) <= 5.0)	
				{
					//	printf(,currPos);
					m_fadeState = FADE_DOWN;
				}
			}
			break;

		default:
			{
				assert(!"undefined state!");
			}
			break;
		};
	};
};
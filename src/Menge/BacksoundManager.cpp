#	include "BacksoundManager.h"

#	include "Playlist.h"

#	include "XmlParser.h"

#	include "FileEngine.h"

#	include "SoundEngine.h"

using namespace Menge;

BacksoundManager::BacksoundManager()
: m_soundSource(0)
, m_fileData(0)
, m_currentPlayList(0)
, m_fadeState(true)
, m_changeTrack(false)
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
		Holder<SoundEngine>::hostage()->deleteSound(m_soundSource);
	}

	if (m_fileData)
	{
		Holder<FileEngine>::hostage()->closeFile(m_fileData);
	}
}

void	BacksoundManager::setFadeTime(double _fadeTime)
{
	m_fadeTime = _fadeTime;
	assert(_fadeTime > 0);
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
		m_fadeState = false;
	}
	else
	{
		_beginFade();
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

bool	BacksoundManager::listenRecycled()
{
	return 0;
}
void	BacksoundManager::listenPaused()
{

}
void	BacksoundManager::listenStopped()
{
	m_currentPlayList->nextSong();
	m_changeTrack = !m_changeTrack;
}

void	BacksoundManager::update(double _timing)
{
//		printf("vol = %f \n",m_soundSource->getGain());
	
	if(m_soundSource == NULL)
	{
		return;
	}

	if(m_changeTrack == true)
	{
		Holder<SoundEngine>::hostage()->deleteSound(m_soundSource);
		m_soundSource = NULL;

		Holder<FileEngine>::hostage()->closeFile(m_fileData);
		m_fileData = NULL;

		_beginFade();
		m_changeTrack = false;
	}

	clock_t ticks = clock();

	double time = (ticks - m_timeFadeBegin) / 1000.0;

	if(m_fadeState == true)
	{
		if (m_soundSource->getGain() >= 1.0f)
		{
			m_soundSource->setGain(1.0f);

			m_timeFadeEnd = clock();
			m_fadeState = false;

			printf("FADE_DOWN \n");
		}
		else
		{
			m_soundSource->setGain(float(time / m_fadeTime));
		}
	}
	else
	{
		if(m_soundSource->getGain() > 0)
		{
			if(time >= m_fadeoffTime)
			{
				m_soundSource->setGain( float((m_fadeoffTime - (ticks - m_timeFadeEnd) / 1000.0 ) / m_fadeTime));
			}
		}
	}
};

void	BacksoundManager::_beginFade()
{
	printf("FADE_UP \n");

	m_fadeState = true;

		Holder<SoundEngine>::hostage()->addSoundNode(
		m_soundSource,
		m_fileData,
		m_currentPlayList->getCurrentSongName(),
		this, true
		);

	m_soundSource->setGain(0);
	m_soundSource->play();

	assert(m_soundSource->getSizeSec() > 2*m_fadeTime);

	m_fadeoffTime = m_soundSource->getSizeSec() - m_fadeTime;

	m_timeFadeBegin = clock();
}

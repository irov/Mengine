#	include "Amplifier.h"

#	include "Playlist.h"

#	include "XmlParser.h"

#	include "FileEngine.h"

#	include "SoundEngine.h"

using namespace Menge;

Amplifier::Amplifier()
: m_soundSource(0)
, m_fileData(0)
, m_currentPlayList(0)
, m_fadeState(true)
, m_changeTrack(false)
, m_isPaused(false)
, m_fadeTime(3) //3 секунды на фейд трека!
{
	assert(m_fadeTime > 0);
	Holder<Amplifier>::keep(this);
}

Amplifier::~Amplifier()
{
	for (TPlayListMap::iterator it = m_playLists.begin(); it!=m_playLists.end(); ++it)
	{
		delete (*it).second;
	}
	Holder<SoundEngine>::hostage()->deleteSound(m_soundSource);
	Holder<FileEngine>::hostage()->closeFile(m_fileData);
}

void	Amplifier::playList(const std::string& _playListName)
{
	TPlayListMap::iterator it = m_playLists.find(_playListName);

	if (it == m_playLists.end())
	{
		assert(!"PlayList not found!");
		return;
	}

	m_currentPlayList = it->second;

	m_currentPlayList->loadTracks();

	Holder<SoundEngine>::hostage()->deleteSound(m_soundSource);
	m_soundSource = NULL;

	Holder<FileEngine>::hostage()->closeFile(m_fileData);
	m_fileData = NULL;

	m_changeTrack = false;

	Holder<SoundEngine>::hostage()->addSoundNode(
		m_soundSource,
		m_fileData,
		m_currentPlayList->getCurrentSongName(),
		this, true
		);

	printf("FADE_UP \n");

	m_soundSource->play();
	m_soundSource->setVolume(0);

	assert(m_soundSource->getLengthS() > 2 * m_fadeTime);

	m_fadeoffTime = m_soundSource->getLengthS() - m_fadeTime;

	m_timeFadeBegin = clock();
}

void	Amplifier::addPlayList(const std::string& _playListFileName)
{
	m_playLists.insert(
		std::make_pair(_playListFileName, new Playlist(_playListFileName))
		);
}

void	Amplifier::erasePlayList(const std::string& _playListFileName)
{
	m_playLists.erase(_playListFileName);
}

void	Amplifier::loadPlayList(const std::string& _filename)
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

void	Amplifier::listenPaused(bool _pause)
{
	m_isPaused = _pause;
}

void	Amplifier::listenStopped()
{
	printf("here i am mother fuckers!\n");
	m_changeTrack = true;
}

void	Amplifier::update(double _timing)
{
	if(m_soundSource == NULL || m_isPaused == true)
	{
		return;
	} 

	//printf("vol = %f \n",m_soundSource->getPositiongetVolume());

	if(m_changeTrack == true)
	{
		m_currentPlayList->nextSong();

		Holder<SoundEngine>::hostage()->deleteSound(m_soundSource);
		m_soundSource = NULL;
		Holder<FileEngine>::hostage()->closeFile(m_fileData);
		m_fileData = NULL;

		_beginFade();
		m_changeTrack = false;
	}

	clock_t ticks = clock();

	double time_offset_begin = (ticks - m_timeFadeBegin) / 1000.0;

	if(m_fadeState == true)
	{
		if (m_soundSource->getVolume() >= 1.0f)
		{
			m_soundSource->setVolume(1.0f);

			m_timeFadeEnd = clock();
			m_fadeState = false;

			printf("FADE_DOWN \n");
		}
		else
		{
			float new_volume = time_offset_begin / m_fadeTime;
			printf("%f \n",new_volume);
			m_soundSource->setVolume(new_volume);
		}
	}
	else
	{
		if(m_soundSource->getVolume() > 0)
		{
			if(time_offset_begin >= m_fadeoffTime)
			{
				double time_offset_end = (ticks - m_timeFadeEnd) / 1000.0;
				float new_volume = (m_fadeoffTime - time_offset_end) / m_fadeTime;
				m_soundSource->setVolume(new_volume);
			}
		}
	}      

}

void	Amplifier::_beginFade()
{
	m_fadeState = true;

	printf("%s \n",m_currentPlayList->getCurrentSongName().c_str());

	Holder<SoundEngine>::hostage()->addSoundNode(
		m_soundSource,
		m_fileData,
		m_currentPlayList->getCurrentSongName(),
		this, true
		);

	printf("FADE_UP \n");

	m_soundSource->play();
	m_soundSource->setVolume(0);

	assert(m_soundSource->getLengthS() > 2 * m_fadeTime);

	m_fadeoffTime = m_soundSource->getLengthS() - m_fadeTime;

	m_timeFadeBegin = clock();
}
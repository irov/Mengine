
#include "stdafx.h"

#include "ALSoundBuffer.h"
#include "ALSoundSource.h"

#include <algorithm>

ALSoundBuffer::ALSoundBuffer() :
m_bufferName(0),
m_lenghtMs(0)
{
	alGenBuffers(1, &m_bufferName);
}

ALSoundBuffer::~ALSoundBuffer()
{
	for(int i = 0; i < m_sources.size(); i++)
	{
		m_sources[i]->stop();
		static_cast<ALSoundSource*>(m_sources[i])->setSoundBuffer(NULL);
	}
	alDeleteBuffers(1, &m_bufferName);
}

void ALSoundBuffer::addSource(SoundSourceInterface *_source)
{
	m_sources.push_back(_source);
}

void ALSoundBuffer::removeSource(SoundSourceInterface *_source)
{
	TVectorSoundSourceInterface::iterator it = std::find(m_sources.begin(), m_sources.end(), _source);
	if(it != m_sources.end())
		m_sources.erase(it);
}

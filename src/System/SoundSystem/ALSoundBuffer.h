
#pragma once

#include "AL.h"

#include "SoundSystemInterface.h"

#include <vector>

class ALSoundBuffer : public SoundBufferInterface
{
public:
	ALSoundBuffer();
	virtual ~ALSoundBuffer();

	virtual bool isStreamed()	const	{ return false; }
	ALuint getBufferName()		const	{ return m_bufferName; }
	unsigned int  getLenghtMs()	const	{ return m_lenghtMs; }

	void setLenghtMs(unsigned int _ms)	{ m_lenghtMs = _ms; }

	void addSource(SoundSourceInterface* _source);// { m_sources.push_back(_source); }
	void removeSource(SoundSourceInterface* _source); //{ m_sources.erase(_source); }

protected:

	typedef std::vector<SoundSourceInterface*>	TVectorSoundSourceInterface;
	TVectorSoundSourceInterface m_sources;
	ALuint m_bufferName;
	unsigned int m_lenghtMs;
};
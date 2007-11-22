
#pragma once

#include "SoundSystemInterface.h"

class ALSoundBuffer : public SoundBufferInterface
{
public:
	ALSoundBuffer();
	virtual ~ALSoundBuffer();

	virtual void unload();

	virtual bool isStreamed()	const	{ return false; }
	ALuint getBufferName()		const	{ return m_bufferName; }
	unsigned int  getLenghtMs()	const	{ return m_lenghtMs; }

	void setLenghtMs(unsigned int _ms)	{ m_lenghtMs = _ms; }

protected:
	ALuint m_bufferName;
	unsigned int m_lenghtMs;
};
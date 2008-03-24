
#pragma once

#include "AL.h"

#include "../../Interface/SoundSystemInterface.h"

#include <vector>
#include <string>

class ALSoundBuffer 
	: public SoundBufferInterface
{
public:
	ALSoundBuffer( const std::string& _filename );
	virtual ~ALSoundBuffer();

	virtual bool isStreamed()	const	{ return false; }
	ALuint getBufferName()		const	{ return m_bufferName; }
	unsigned int  getLenghtMs()	const;

	void setLenghtMs(unsigned int _ms)	{ m_lenghtMs = _ms; }

	void addSource(SoundSourceInterface* _source);
	void removeSource(SoundSourceInterface* _source); 

protected:

	typedef std::vector<SoundSourceInterface*>	TVectorSoundSourceInterface;
	TVectorSoundSourceInterface m_sources;
	ALuint m_bufferName;
	unsigned int m_lenghtMs;
	std::string m_filename;
};

#pragma once

#include "AL.h"

#include "../../Interface/SoundSystemInterface.h"

#include <vector>
#include <string>

class ALSoundBuffer 
	: public SoundBufferInterface
{
public:
	ALSoundBuffer();
	virtual ~ALSoundBuffer();

	virtual bool isStreamed()	const	{ return false; }
	ALuint getBufferName()		const	{ return m_alID; }
	unsigned int  getLenghtMs()	const;

	void setLenghtMs(unsigned int _ms)	{ m_lenghtMs = _ms; }

	void addSource(SoundSourceInterface* _source);
	void removeSource(SoundSourceInterface* _source);

	const std::string& getFilename()	{ return m_filename; }

	bool loadOgg( const char* _filename );

protected:

	typedef std::vector<SoundSourceInterface*>	TVectorSoundSourceInterface;
	TVectorSoundSourceInterface m_sources;
	ALuint m_alID;
	unsigned int m_lenghtMs;
	std::string m_filename;
	bool m_isEmpty;
};
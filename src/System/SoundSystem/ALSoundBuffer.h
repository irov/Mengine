
#pragma once

#	include "AL/AL.h"

#	include "../../Interface/SoundSystemInterface.h"

#	include <vector>
#	include "vorbis/vorbisfile.h"

class ALSoundBuffer 
	: public SoundBufferInterface
{
public:
	ALSoundBuffer();
	virtual ~ALSoundBuffer();

	virtual bool isStreamed()	const;
	ALuint getAlID()			const;
	unsigned int  getLenghtMs()	const;

	void addSource(SoundSourceInterface* _source);
	void removeSource(SoundSourceInterface* _source);

	virtual bool loadOgg( const char* _filename );
	const std::string& getFilename()	{ return m_filename; }

	bool isStereo();

protected:

	typedef std::vector<SoundSourceInterface*>	TVectorSoundSourceInterface;
	TVectorSoundSourceInterface m_sources;
	ALuint m_alID;
	unsigned int m_lenghtMs;
	std::string m_filename;
	bool m_isEmpty;
	bool m_isStereo;

	unsigned int decodeOggVorbis_( OggVorbis_File* stream, char* _buffer, unsigned int _bufferSize );
};
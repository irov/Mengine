
#pragma once

#	include "ALSoundBuffer.h"

#	include "Vorbis/Vorbisfile.h"
#	include <fstream>

//////////////////////////////////////////////////////////////////////////
class ALSoundBufferStream 
	: public ALSoundBuffer
{
public:
	ALSoundBufferStream();
	virtual ~ALSoundBufferStream();

	bool isStreamed() const;
	bool loadOgg( const char* _filename );

	void start( ALuint source );
	bool update();
	void stop();

protected:

	ALuint m_alID2;

	// OpenAL format of the sound data.
	ALenum m_format;
	// Frequency of the sound data.
	ALuint m_frequency;
	ALuint m_channels;
	//Source to update.
	ALuint m_source;

	char* m_buffer;
	std::ifstream m_stream;
	OggVorbis_File m_oggFile;	// The file structure
	unsigned int m_bufferSize;	// Size of the buffer in bytes
	bool m_looping;				// Are we looping or not?
	bool m_updating;
};
//////////////////////////////////////////////////////////////////////////
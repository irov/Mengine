
#pragma once

#include "ALSoundBuffer.h"

//#include "PThread.h"
#include "Vorbis/Vorbisfile.h"

//#define PROCESSING_TIME 20

class ALSoundBufferStreamUpdater
{
public:
	ALSoundBufferStreamUpdater(const OggVorbis_File& oggfile, ALuint buffer1, ALuint buffer2, ALenum format, unsigned int frequency, unsigned int _buffersize, unsigned int _channels);

	void start(ALuint sourcename);
  
	// Remove a source from the stream.
	void stop();

	static void* run(void*);

	void update();

	void setLooping(bool _looping)	{ m_looping = _looping; }
	bool isLooping()				{ return m_looping; }

//protected:

	unsigned int decodeOggVorbis(OggVorbis_File* _oggVorbisFile, char* _decodeBuffer, unsigned int _bufferSize, unsigned int _channels);
	// Destructor.
	virtual ~ALSoundBufferStreamUpdater();


	// Names of the buffers to update
	ALuint m_buffers[2];

	// OpenAL format of the sound data.
	ALenum m_format;
	
	// Frequency of the sound data.
	unsigned int m_frequency;
	unsigned int m_channels;
	//Source to update.
	ALuint m_source;

	//Flag for when Run should stop running..
	//volatile bool m_stopRunning;

	//pthread_t m_thread;
	char* m_buffer;
	OggVorbis_File* m_oggFile;	// The file structure
	unsigned int m_bufferSize;	// Size of the buffer in bytes
	bool m_looping;				// Are we looping or not?
	bool m_updating;
};


class ALSoundBufferStream : public ALSoundBuffer
{
public:
	ALSoundBufferStream();
	ALSoundBufferStream(const char* _filename);
	virtual ~ALSoundBufferStream();

	virtual bool isStreamed() const	{ return true; }

	void record(ALuint sourcename);
	void stop();

	ALSoundBufferStreamUpdater* getUpdater() { return m_updater; }

private:
	ALSoundBufferStreamUpdater* m_updater;
	ALuint m_buffer2;
};
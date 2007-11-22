
#pragma once

#include "ALSoundBuffer.h"

#include "OpenThreads/Thread"
#include "OpenThreads/Mutex"
#include "Vorbis/Vorbisfile.h"

class ALSoundBufferStreamUpdater : public OpenThreads::Thread, public OpenThreads::Mutex
{
public:
	ALSoundBufferStreamUpdater(const OggVorbis_File& oggfile, ALuint buffer1, ALuint buffer2, ALenum format, unsigned int frequency, unsigned int _buffersize);

	void addSource(ALuint sourcename);
  
	// Remove a source from the stream.
	void removeSource();

	void run();

	bool update(void* _buffer, unsigned int _length); 

	// Inherited from Thread.
	// Is called after run() finishes, and deletes this.
	void cancelCleanup();

	void setLooping(bool _looping)	{ m_looping = _looping; }
	bool isLooping()				{ return m_looping; }

protected:

	// Destructor.
	virtual ~ALSoundBufferStreamUpdater();


	// Names of the buffers to update
	ALuint m_buffers[2];

	// OpenAL format of the sound data.
	ALenum m_format;

	// Frequency of the sound data.
	unsigned int m_frequency;

	//Source to update.
	ALuint m_source;

	bool m_newSource, m_removeSource;

	//Flag for when Run should stop running..
	bool m_stopRunning;

	// Mutex for mStopRunning.
	OpenThreads::Mutex m_runMutex;

	OggVorbis_File* m_oggFile;	// The file structure
	unsigned int m_bufferSize;	// Size of the buffer in bytes
	bool m_looping;				// Are we looping or not?

};


class ALSoundBufferStream : public ALSoundBuffer
{
public:
	ALSoundBufferStream();
	ALSoundBufferStream(const char* _filename);
	virtual ~ALSoundBufferStream();

	virtual void unload();

	virtual bool isStreamed() const	{ return true; }

	void record(ALuint sourcename);
	void stop();

	ALSoundBufferStreamUpdater* getUpdater() { return m_updater; }

private:
	ALSoundBufferStreamUpdater* m_updater;
	ALuint m_buffer2;
};
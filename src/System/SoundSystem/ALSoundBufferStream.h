
#pragma once

#include "ALSoundBuffer.h"

#include "OpenThreads/Thread"
#include "OpenThreads/Mutex"
#include "Vorbis/Vorbisfile.h"

class ALSoundBufferStreamUpdater : public OpenThreads::Thread, public OpenThreads::Mutex
{
public:
	ALSoundBufferStreamUpdater(const OggVorbis_File& oggfile, ALuint buffer1, ALuint buffer2, ALenum format, unsigned int frequency, UINT _buffersize);

	void addSource(ALuint sourcename);
  
	// Remove a source from the stream.
	void removeSource();

	void run();

	bool update(void* _buffer, UINT _length); 

	// Inherited from Thread.
	// Is called after run() finishes, and deletes this.
	void cancelCleanup();

	void setLooping(bool _looping)	{ mLooping = _looping; }
	bool isLooping()				{ return mLooping; }

protected:

	// Destructor.
	virtual ~ALSoundBufferStreamUpdater();


	// Names of the buffers to update
	ALuint mBuffers[2];

	// OpenAL format of the sound data.
	ALenum mFormat;

	// Frequency of the sound data.
	UINT mFrequency;

	//Source to update.
	ALuint mSource;

	bool mNewSource, mRemoveSource;
	//std::vector<ALuint> sources_,newsources_,removesources_;

	//Flag for when Run should stop running..
	bool mStopRunning;

	// Mutex for mStopRunning.
	OpenThreads::Mutex mRunMutex;

	OggVorbis_File* mOggFile;	// The file structure
	UINT mBufferSize;			// Size of the buffer in bytes
	bool mLooping;				// Are we looping or not?

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

	ALSoundBufferStreamUpdater* getUpdater() { return mUpdater; }

private:
	ALSoundBufferStreamUpdater* mUpdater;
	ALuint mBuffer2;
};
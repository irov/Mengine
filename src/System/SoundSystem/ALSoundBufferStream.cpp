
#include "stdafx.h"

#include "ALSoundBufferStream.h"

#include "AL/Alut.h"

#define ENTER_CRITICAL lock()
#define LEAVE_CRITICAL unlock()

ALSoundBufferStreamUpdater::ALSoundBufferStreamUpdater(const OggVorbis_File& _oggfile, ALuint _buffer1, ALuint _buffer2, ALenum _format, UINT _frequency, UINT _buffersize) :
mSource(AL_INVALID),
mFormat(_format),
mFrequency(_frequency),
mStopRunning(false),
mNewSource(false),
mRemoveSource(false),
mLooping(false),
mBufferSize(_buffersize)
{
  mBuffers[0] = _buffer1;
  mBuffers[1] = _buffer2;
  mOggFile = new OggVorbis_File(_oggfile);
  Init();

}

ALSoundBufferStreamUpdater::~ALSoundBufferStreamUpdater() 
{
	if(mOggFile)
		ov_clear(mOggFile);
	delete mOggFile;
	
	mRunMutex.lock();
	mStopRunning = true;
	mRunMutex.unlock();

	cancel();
}

void ALSoundBufferStreamUpdater::addSource(ALuint _sourcename) 
{
	alSourceStop(_sourcename);
	ENTER_CRITICAL;
	mSource = _sourcename;
	mNewSource = true;
	start();
	LEAVE_CRITICAL;
}

void ALSoundBufferStreamUpdater::removeSource() 
{
  ENTER_CRITICAL;
  mRemoveSource = true;
  LEAVE_CRITICAL;
}

void ALSoundBufferStreamUpdater::run()
{
	char* buffer = new char[mBufferSize];
	mRunMutex.lock();
	while(!mStopRunning) 
	{
		mRunMutex.unlock();

		UINT count = 0;
		int stream;
		UINT amt = 0;
		while (amt = ov_read(mOggFile, buffer + count, mBufferSize - count, 0, 2, 1, &stream))
		{
			count += amt;

			if (count >= mBufferSize)
				break;
		}
		if(count)
		  update(buffer, count);

		mRunMutex.lock();
	}

	mRunMutex.unlock();
	delete []buffer;
}

bool ALSoundBufferStreamUpdater::update(void* _buffer, UINT _length) 
{
	if(!(_length && _buffer))     // Zero length or NULL pointer => return
		return false;

	ALint processed,state;
	ALuint albuffer;

	ENTER_CRITICAL;

	if( mRemoveSource && (mSource != AL_INVALID) ) 
	{
		alSourceStop(mSource);
		ALuint dump[2];
		ALint nqueued;
		alGetSourceiv(mSource, AL_BUFFERS_QUEUED, &nqueued);
		if(nqueued)
			alSourceUnqueueBuffers(mSource, nqueued, dump);
		alGetError();
		mSource = AL_INVALID;
		mRemoveSource = false;
	}

	if(mNewSource) 
	{
		if(mSource != AL_INVALID)
			alSourceStop(mSource);
		mNewSource = false;

		LEAVE_CRITICAL;
		Sleep(50);
		ENTER_CRITICAL;
	}

	processed = 0;
	while(!processed) 
	{
		alGetSourceiv(mSource, AL_SOURCE_STATE, &state);

		if(state != AL_PLAYING) 
		{
			ALuint dump[2];
			alSourceStop(mSource);
			ALint nqueued;
			alGetSourceiv(mSource, AL_BUFFERS_QUEUED, &nqueued);
			if(nqueued)
				alSourceUnqueueBuffers(mSource, nqueued, dump);
			alBufferData(mBuffers[0], mFormat, _buffer, _length , mFrequency);
			alBufferData(mBuffers[1], mFormat, (char *)_buffer + _length / 2, _length / 2, mFrequency);

			alSourceQueueBuffers(mSource, 2, mBuffers);

			alSourcePlay(mSource);

			processed = 1;
		} 
		else 
		{
			processed = 1;
			alGetSourceiv(mSource, AL_BUFFERS_PROCESSED, &processed);

			if(processed) 
			{
				alSourceUnqueueBuffers(mSource, 1, &albuffer);
				alBufferData(albuffer, mFormat, _buffer, _length, mFrequency);
				alSourceQueueBuffers(mSource, 1, &albuffer);
			} 
			else 
			{
				LEAVE_CRITICAL;
				YieldCurrentThread();
				ENTER_CRITICAL;

				if(mLooping)
					ov_time_seek(mOggFile, 0.0);

				// Not sure if this is necessary, but just in case...
				if(mRemoveSource) 
				{
					LEAVE_CRITICAL;
					return update(_buffer, _length);
				}
			} // if(processed) else
		} // if(state!=AL_PLAYING) else
	} // while(!processed)

	LEAVE_CRITICAL;
	bool ret;
	mRunMutex.lock();
	ret = mStopRunning;
	mRunMutex.unlock();
    return ret;
}

void ALSoundBufferStreamUpdater::cancelCleanup() 
{
  //std::cerr << "StreamUpdater::cancelCleanup: Should probably not delete this" << std::endl;
  delete this;
} 

ALSoundBufferStream::ALSoundBufferStream()
{
}

ALSoundBufferStream::ALSoundBufferStream(const char* _filename)
{

	alGenBuffers(1, &mBuffer2);

  FILE *filehandle = fopen(_filename, "rb");

  UINT buffersize, format, freq, size;
  // Check for file type, create a FileStreamUpdater if a known type is
  // detected, otherwise throw an error.
  OggVorbis_File oggfile;
  if(ov_open(filehandle, &oggfile, NULL, 0) >= 0) 
  {
    vorbis_info *ogginfo = ov_info(&oggfile, -1);
	freq = ogginfo->rate;
	mLenghtMs = ov_time_total(&oggfile, -1) * 1000;
	if (ogginfo->channels == 1)
	{
		format = AL_FORMAT_MONO16;
		// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
		buffersize = freq >> 1;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		buffersize -= (buffersize % 2);
	}
	else if (ogginfo->channels == 2)
	{
		format = AL_FORMAT_STEREO16;
		// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
		buffersize = freq;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		buffersize -= (buffersize % 4);
	}
	else if (ogginfo->channels == 4)
	{
		format = alGetEnumValue("AL_FORMAT_QUAD16");
		// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
		buffersize = freq * 2;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		buffersize -= (buffersize % 8);
	}
	else if (ogginfo->channels == 6)
	{
		format = alGetEnumValue("AL_FORMAT_51CHN16");
		// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
		buffersize = freq * 3;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		buffersize -= (buffersize % 12);
	}
    
    mUpdater = new ALSoundBufferStreamUpdater(oggfile, getBufferName(), mBuffer2, format, ogginfo->rate, buffersize * 2/**sampleSize(format)*/); 
  } 
  //else 
  //{
  //  fclose(filehandle);
  //}
  //ov_clear(&oggfile);
  //fclose(filehandle);
}

ALSoundBufferStream::~ALSoundBufferStream()
{
}

void ALSoundBufferStream::unload()
{
	alDeleteBuffers(1, &mBuffer2);
}

void ALSoundBufferStream::record(ALuint sourcename)
{
  mUpdater->addSource(sourcename);
//	mUpdater->run();
}

void ALSoundBufferStream::stop()
{
	mUpdater->removeSource();
}
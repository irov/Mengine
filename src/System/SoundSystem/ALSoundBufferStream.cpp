
#include "stdafx.h"

#include "ALSoundBufferStream.h"

#include "AL/Alut.h"

#define ENTER_CRITICAL lock()
#define LEAVE_CRITICAL unlock()

ALSoundBufferStreamUpdater::ALSoundBufferStreamUpdater(const OggVorbis_File& _oggfile, ALuint _buffer1, ALuint _buffer2, ALenum _format, unsigned int _frequency, unsigned int _buffersize) :
m_source(AL_INVALID),
m_format(_format),
m_frequency(_frequency),
m_stopRunning(false),
m_newSource(false),
m_removeSource(false),
m_looping(false),
m_bufferSize(_buffersize)
{
  m_buffers[0] = _buffer1;
  m_buffers[1] = _buffer2;
  m_oggFile = new OggVorbis_File(_oggfile);
  Init();

}

ALSoundBufferStreamUpdater::~ALSoundBufferStreamUpdater() 
{
	if(m_oggFile)
		ov_clear(m_oggFile);
	delete m_oggFile;
	
	m_runMutex.lock();
	m_stopRunning = true;
	m_runMutex.unlock();

	cancel();
}

void ALSoundBufferStreamUpdater::addSource(ALuint _sourcename) 
{
	alSourceStop(_sourcename);
	ENTER_CRITICAL;
	m_source = _sourcename;
	m_newSource = true;
	start();
	LEAVE_CRITICAL;
}

void ALSoundBufferStreamUpdater::removeSource() 
{
  ENTER_CRITICAL;
  m_removeSource = true;
  LEAVE_CRITICAL;
}

void ALSoundBufferStreamUpdater::run()
{
	char* buffer = new char[m_bufferSize];
	m_runMutex.lock();
	while(!m_stopRunning) 
	{
		m_runMutex.unlock();

		unsigned int count = 0;
		int stream;
		unsigned int amt = 0;
		while (amt = ov_read(m_oggFile, buffer + count, m_bufferSize - count, 0, 2, 1, &stream))
		{
			count += amt;

			if (count >= m_bufferSize)
				break;
		}
		if(count)
		  update(buffer, count);

		m_runMutex.lock();
	}

	m_runMutex.unlock();
	delete []buffer;
}

bool ALSoundBufferStreamUpdater::update(void* _buffer, unsigned int _length) 
{
	if(!(_length && _buffer))     // Zero length or NULL pointer => return
		return false;

	ALint processed,state;
	ALuint albuffer;

	ENTER_CRITICAL;

	if( m_removeSource && (m_source != AL_INVALID) ) 
	{
		alSourceStop(m_source);
		ALuint dump[2];
		ALint nqueued;
		alGetSourceiv(m_source, AL_BUFFERS_QUEUED, &nqueued);
		if(nqueued)
			alSourceUnqueueBuffers(m_source, nqueued, dump);
		alGetError();
		m_source = AL_INVALID;
		m_removeSource = false;
	}

	if(m_newSource) 
	{
		if(m_source != AL_INVALID)
			alSourceStop(m_source);
		m_newSource = false;

		//LEAVE_CRITICAL;
		//Sleep(50);
		//ENTER_CRITICAL;
	}

	processed = 0;
	while(!processed) 
	{
		alGetSourceiv(m_source, AL_SOURCE_STATE, &state);

		if(state != AL_PLAYING) 
		{
			ALuint dump[2];
			alSourceStop(m_source);
			ALint nqueued;
			alGetSourceiv(m_source, AL_BUFFERS_QUEUED, &nqueued);
			if(nqueued)
				alSourceUnqueueBuffers(m_source, nqueued, dump);
			alBufferData(m_buffers[0], m_format, _buffer, _length / 2, m_frequency);
			alBufferData(m_buffers[1], m_format, (char *)_buffer + _length / 2, _length / 2, m_frequency);

			alSourceQueueBuffers(m_source, 2, m_buffers);

			alSourcePlay(m_source);

			processed = 1;
		} 
		else 
		{
			processed = 1;
			alGetSourceiv(m_source, AL_BUFFERS_PROCESSED, &processed);

			if(processed) 
			{
				alSourceUnqueueBuffers(m_source, 1, &albuffer);
				alBufferData(albuffer, m_format, _buffer, _length, m_frequency);
				alSourceQueueBuffers(m_source, 1, &albuffer);
			} 
			else 
			{
				LEAVE_CRITICAL;
				YieldCurrentThread();
				ENTER_CRITICAL;

				if(m_looping)
					ov_time_seek(m_oggFile, 0.0);

				// Not sure if this is necessary, but just in case...
				if(m_removeSource) 
				{
					LEAVE_CRITICAL;
					return update(_buffer, _length);
				}
			} // if(processed) else
		} // if(state!=AL_PLAYING) else
	} // while(!processed)

	LEAVE_CRITICAL;
	bool ret;
	m_runMutex.lock();
	ret = m_stopRunning;
	m_runMutex.unlock();
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

	alGenBuffers(1, &m_buffer2);

  FILE *filehandle = fopen(_filename, "rb");

  unsigned int buffersize, format, freq;
  // Check for file type, create a FileStreamUpdater if a known type is
  // detected, otherwise throw an error.
  OggVorbis_File oggfile;
  if(ov_open(filehandle, &oggfile, NULL, 0) >= 0) 
  {
    vorbis_info *ogginfo = ov_info(&oggfile, -1);
	freq = ogginfo->rate;
	m_lenghtMs = static_cast<unsigned int>( ov_time_total(&oggfile, -1) * 1000 );
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
    
    m_updater = new ALSoundBufferStreamUpdater(oggfile, getBufferName(), m_buffer2, format, ogginfo->rate, buffersize * 2/**sampleSize(format)*/); 
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
	alDeleteBuffers(1, &m_buffer2);
}

void ALSoundBufferStream::record(ALuint sourcename)
{
  m_updater->addSource(sourcename);
//	mUpdater->run();
}

void ALSoundBufferStream::stop()
{
	m_updater->removeSource();
}
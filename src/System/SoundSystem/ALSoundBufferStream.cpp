
#include "ALSoundBufferStream.h"

#include "AL/Alut.h"
#include <windows.h>

ALSoundBufferStreamUpdater::ALSoundBufferStreamUpdater(const OggVorbis_File& _oggfile, ALuint _buffer1, ALuint _buffer2, ALenum _format, unsigned int _frequency, unsigned int _buffersize, unsigned int _channels) 
:m_source( 0 )
,m_buffer( NULL )
,m_format( _format )
,m_frequency( _frequency )
//,m_stopRunning( false )
,m_looping( false )
,m_bufferSize( _buffersize )
,m_channels( _channels )
,m_updating( false )
{
	m_buffers[0] = _buffer1;
	m_buffers[1] = _buffer2;
	m_oggFile = new OggVorbis_File( _oggfile );
}

ALSoundBufferStreamUpdater::~ALSoundBufferStreamUpdater() 
{
	//m_stopRunning = true;
	// wait until thread terminates
	//pthread_join(m_thread, NULL);

	if(m_oggFile)
		ov_clear(m_oggFile);
	delete m_oggFile;

}

void ALSoundBufferStreamUpdater::start(ALuint _sourcename) 
{
	m_source = _sourcename;
	//pthread_create(&m_thread, NULL, &ALSoundBufferStreamUpdater::run, (void*)this);
	// detach all buffers if any
	alSourceStop( m_source );
	alSourcei( m_source, AL_BUFFER, NULL );

	// Fill all the Buffers with decoded audio data from the OggVorbis file
	m_buffer = new char[m_bufferSize];
	unsigned int count = 0;
	unsigned int amt = 0;
	unsigned long bytesWritten = 0;
	for (int i = 0; i < 2; i++)
	{
		bytesWritten = decodeOggVorbis( m_oggFile, m_buffer, m_bufferSize, m_channels );
		if (bytesWritten)
		{
			alBufferData( m_buffers[i], m_format, m_buffer, bytesWritten, m_frequency );
			alSourceQueueBuffers( m_source, 1, &m_buffers[i] );
		}
	}

	// Start playing source
	alSourcePlay(m_source);

	m_updating = true;
}

void ALSoundBufferStreamUpdater::stop() 
{
	//m_stopRunning = true;
	// wait until thread terminates
	//pthread_join(m_thread, NULL);
	// Stop the Source and clear the Queue
	alSourceStop( m_source );
	alSourcei( m_source, AL_BUFFER, 0 );

	if ( m_buffer )
	{
		delete m_buffer;
		m_buffer = NULL;
	}

	m_updating = false;
}

/*void* ALSoundBufferStreamUpdater::run(void* _this)
{
	ALSoundBufferStreamUpdater* pThis = static_cast<ALSoundBufferStreamUpdater*>(_this);

	// detach all buffers if any
	alSourceStop(pThis->m_source);
	alSourcei(pThis->m_source, AL_BUFFER, NULL);

	// Fill all the Buffers with decoded audio data from the OggVorbis file
	char* buffer = new char[pThis->m_bufferSize];
	unsigned int count = 0;
	int stream;
	unsigned int amt = 0;
	unsigned long bytesWritten = 0;
	for (int i = 0; i < 2; i++)
	{
		bytesWritten = pThis->decodeOggVorbis( pThis->m_oggFile, buffer, pThis->m_bufferSize, pThis->m_channels );
		if (bytesWritten)
		{
			alBufferData(pThis->m_buffers[i], pThis->m_format, buffer, bytesWritten, pThis->m_frequency);
			alSourceQueueBuffers(pThis->m_source, 1, &pThis->m_buffers[i]);
		}
	}

	// Start playing source
	alSourcePlay(pThis->m_source);

	int buffersProcessed;
	int queuedBuffers;
	int state;
	ALuint albuffer;

	while (!pThis->m_stopRunning)
	{
		// wait some time while buffers processing
		Sleep( PROCESSING_TIME );

		// Request the number of OpenAL Buffers have been processed (played) on the Source
		buffersProcessed = 0;
		alGetSourcei(pThis->m_source, AL_BUFFERS_PROCESSED, &buffersProcessed);

		// For each processed buffer, remove it from the Source Queue, read next chunk of audio
		// data from disk, fill buffer with new data, and add it to the Source Queue
		while (buffersProcessed)
		{
			printf( "Updating stream\n" );
			// Remove the Buffer from the Queue.  (uiBuffer contains the Buffer ID for the unqueued Buffer)
			albuffer = 0;
			alSourceUnqueueBuffers(pThis->m_source, 1, &albuffer);

			// Read more audio data (if there is any)
			bytesWritten = pThis->decodeOggVorbis(pThis->m_oggFile, buffer, pThis->m_bufferSize, pThis->m_channels);

			if ( !bytesWritten )
			{
				// we reached end of file
				if( pThis->m_looping )
				{
					ov_time_seek( pThis->m_oggFile, 0.0 );
					bytesWritten = pThis->decodeOggVorbis(pThis->m_oggFile, buffer, pThis->m_bufferSize, pThis->m_channels);
				}
				else
				{
					break;
				}
			}
			
			alBufferData(albuffer, pThis->m_format, buffer, bytesWritten, pThis->m_frequency);
			alSourceQueueBuffers(pThis->m_source, 1, &albuffer);
			buffersProcessed--;
		}

		// Check the status of the Source.  If it is not playing, then playback was completed,
		// or the Source was starved of audio data, and needs to be restarted.
		alGetSourcei(pThis->m_source, AL_SOURCE_STATE, &state);

		if (state != AL_PLAYING)
		{
			// If there are Buffers in the Source Queue then the Source was starved of audio
			// data, so needs to be restarted (because there is more audio data to play)
			alGetSourcei(pThis->m_source, AL_BUFFERS_QUEUED, &queuedBuffers);

			if (queuedBuffers)
			{
				alSourcePlay(pThis->m_source);
			}
			else
			{
				// Finished playing
				break;
			}
		}
	}
	// Stop the Source and clear the Queue
	alSourceStop(pThis->m_source);
	alSourcei(pThis->m_source, AL_BUFFER, 0);

	if (buffer)
	{
		delete buffer;
		buffer = NULL;
	}

	return 0;
}*/

void ALSoundBufferStreamUpdater::update()
{
	if( !m_updating ) return;

	int buffersProcessed;
	int queuedBuffers;
	int state;
	ALuint albuffer;
	unsigned long bytesWritten = 0;

	// Request the number of OpenAL Buffers have been processed (played) on the Source
	buffersProcessed = 0;
	alGetSourcei( m_source, AL_BUFFERS_PROCESSED, &buffersProcessed );
	ALenum error = alGetError();
	if( alGetError() != AL_FALSE )
	{
		printf( "ALERROR!" );
	}

	// For each processed buffer, remove it from the Source Queue, read next chunk of audio
	// data from disk, fill buffer with new data, and add it to the Source Queue
	while (buffersProcessed)
	{
		// Remove the Buffer from the Queue.  (uiBuffer contains the Buffer ID for the unqueued Buffer)
		albuffer = 0;
		alSourceUnqueueBuffers( m_source, 1, &albuffer );
		error = alGetError();
		if( error != AL_FALSE )
		{
			printf( "ALERROR!" );
		}

		// Read more audio data (if there is any)
		bytesWritten = decodeOggVorbis( m_oggFile, m_buffer, m_bufferSize, m_channels );

		if ( !bytesWritten )
		{
			// we reached end of file
			if( m_looping )
			{
				ov_time_seek( m_oggFile, 0.0 );
				bytesWritten = decodeOggVorbis( m_oggFile, m_buffer, m_bufferSize, m_channels );
			}
			else
			{
				break;
			}
		}

		alBufferData( albuffer, m_format, m_buffer, bytesWritten, m_frequency );
		error = alGetError();
		if( error != AL_FALSE )
		{
			printf( "ALERROR!" );
		}
		alSourceQueueBuffers( m_source, 1, &albuffer );
		error = alGetError();
		if( error != AL_FALSE )
		{
			//printf( "ALERROR!" );
			if( error == AL_INVALID_OPERATION )
			{
				alSourcei( m_source, AL_BUFFER, NULL );
				alSourceQueueBuffers( m_source, 1, &albuffer );
			}
		}
		buffersProcessed--;
	}

	// Check the status of the Source.  If it is not playing, then playback was completed,
	// or the Source was starved of audio data, and needs to be restarted.
	alGetSourcei( m_source, AL_SOURCE_STATE, &state );
	error = alGetError();
	if( error != AL_FALSE )
	{
		printf( "ALERROR!" );
	}

	if (state != AL_PLAYING)
	{
		// If there are Buffers in the Source Queue then the Source was starved of audio
		// data, so needs to be restarted (because there is more audio data to play)
		alGetSourcei( m_source, AL_BUFFERS_QUEUED, &queuedBuffers );
		if( alGetError() != AL_FALSE )
		{
			printf( "ALERROR!" );
		}

		if ( queuedBuffers )
		{
			alSourcePlay( m_source );
		}
		else
		{
			// Finished playing
			//break;
			m_updating = false;
		}
	}

}

unsigned int ALSoundBufferStreamUpdater::decodeOggVorbis(OggVorbis_File* _oggVorbisFile, char* _decodeBuffer, unsigned int _bufferSize, unsigned int _channels)
{
	int current_section;
	long decodeSize;
	//unsigned long samplesNum;
	//short *samples;

	unsigned long bytesDone = 0;
	while (decodeSize = ov_read(_oggVorbisFile, _decodeBuffer + bytesDone, _bufferSize - bytesDone, 0, 2, 1, &current_section))
	{
		bytesDone += decodeSize;

		if (bytesDone >= _bufferSize)
			break;
	}

	return bytesDone;
}

ALSoundBufferStream::ALSoundBufferStream( const std::string& _filename ) 
: ALSoundBuffer( _filename )
, m_updater(NULL)
, m_buffer2(0)
{
	alGenBuffers(1, &m_buffer2);
}
//////////////////////////////////////////////////////////////////////////
ALSoundBufferStream::~ALSoundBufferStream()
{
	delete m_updater;

	alDeleteBuffers(1, &m_buffer2);
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundBufferStream::initialize( const char* _filename )
{
	//alGenBuffers(1, &m_buffer2);

	FILE * filehandle = 0;
	fopen_s( &filehandle, _filename, "rb");

	if( filehandle == 0 )
	{
		return false;
	}

	unsigned int buffersize, format, freq;
	// Check for file type, create a FileStreamUpdater if a known type is
	// detected, otherwise throw an error.
	OggVorbis_File oggfile;
	if( ov_open(filehandle, &oggfile, NULL, 0) < 0 ) 
	{
		return false;
	}

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

	m_updater = new ALSoundBufferStreamUpdater( oggfile, getBufferName(), m_buffer2, format, ogginfo->rate, buffersize, ogginfo->channels ); 

	return true;
}

void ALSoundBufferStream::record(ALuint sourcename)
{
  m_updater->start(sourcename);
}

void ALSoundBufferStream::stop()
{
	m_updater->stop();
}
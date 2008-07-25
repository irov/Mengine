
#	include "ALSoundBufferStream.h"

#	include "ALSoundSource.h"
#	include "VorbisCallback.h"

//////////////////////////////////////////////////////////////////////////
ALSoundBufferStream::ALSoundBufferStream() 
: ALSoundBuffer()
, m_alID2( 0 )
, m_source( 0 )
{
}
//////////////////////////////////////////////////////////////////////////
ALSoundBufferStream::~ALSoundBufferStream()
{
	if( !m_isEmpty )
	{
		alDeleteBuffers(1, &m_alID);
		alDeleteBuffers(1, &m_alID2);
		ov_clear( &m_oggFile );

		if( m_stream.is_open() )
		{
			m_stream.close();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundBufferStream::isStreamed() const
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundBufferStream::loadOgg( const char* _filename )
{
	if ( !m_isEmpty )
	{
		return false;
	}

	alGenBuffers( 1, &m_alID );
	alGenBuffers( 1, &m_alID2 );

	int error;

	if ( (error = alGetError()) != AL_NO_ERROR )
	{
		return false;
	}

	// callbacks used to read the file
	ov_callbacks vorbisCallbacks;

	// Fill vorbisCallbacks struct
	vorbisCallbacks.read_func  = s_readOgg;
	vorbisCallbacks.seek_func  = s_seekOgg;
	vorbisCallbacks.tell_func  = s_tellOgg;
	vorbisCallbacks.close_func = s_closeOgg;

	// Open Ogg file
	m_stream.open( _filename, std::ios_base::in | std::ios_base::binary );

	if ( ov_open_callbacks( &m_stream, &m_oggFile, NULL, 0, vorbisCallbacks ) < 0 )
		return false;

	vorbis_info *ogginfo = ov_info(&m_oggFile, -1);
	m_frequency = ogginfo->rate;
	m_lenghtMs = static_cast<unsigned int>( ov_time_total(&m_oggFile, -1) * 1000 );
	m_channels = ogginfo->channels;
	if (m_channels == 1)
	{
		m_format = AL_FORMAT_MONO16;
		// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
		m_bufferSize = m_frequency >> 1;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		m_bufferSize -= (m_bufferSize % 2);
	}
	else if (m_channels == 2)
	{
		m_format = AL_FORMAT_STEREO16;
		// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
		m_bufferSize = m_frequency;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		m_bufferSize -= (m_bufferSize % 4);
		m_isStereo = true;
	}
	else if (m_channels == 4)
	{
		m_format = alGetEnumValue("AL_FORMAT_QUAD16");
		// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
		m_bufferSize = m_frequency * 2;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		m_bufferSize -= (m_bufferSize % 8);
		m_isStereo = true;
	}
	else if (m_channels == 6)
	{
		m_format = alGetEnumValue("AL_FORMAT_51CHN16");
		// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
		m_bufferSize = m_frequency * 3;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		m_bufferSize -= (m_bufferSize % 12);
		m_isStereo = true;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundBufferStream::update()
{
	unsigned long bytesWritten = 0;

	int processed = 0;

	ALuint buffer;

	// Получаем количество отработанных буферов
	alGetSourcei( m_source, AL_BUFFERS_PROCESSED, &processed );

	// Если таковые существуют то
	while( processed-- )
	{
		// Исключаем их из очереди
		alSourceUnqueueBuffers( m_source, 1, &buffer );

		// Читаем очередную порцию данных
		bytesWritten = decodeOggVorbis_( &m_oggFile, m_buffer, m_bufferSize );
		alBufferData( buffer, m_format, m_buffer, bytesWritten, m_frequency );

		if ( bytesWritten )
		{
			// Включаем буфер обратно в очередь
			alSourceQueueBuffers( m_source, 1, &buffer );
		}
	}

	// Check the status of the Source.  If it is not playing, then playback was completed,
	// or the Source was starved of audio data, and needs to be restarted.
	int state;
	alGetSourcei( m_source, AL_SOURCE_STATE, &state );
	if (state != AL_PLAYING)
	{
		// If there are Buffers in the Source Queue then the Source was starved of audio
		// data, so needs to be restarted (because there is more audio data to play)
		int queuedBuffers;
		alGetSourcei( m_source, AL_BUFFERS_QUEUED, &queuedBuffers );
		if ( queuedBuffers )
		{
			alSourcePlay( m_source );
		}
	}


	return ( bytesWritten != 0 );
}
//////////////////////////////////////////////////////////////////////////
void ALSoundBufferStream::start( ALuint source )
{
	m_source = source;
	
	alSourcei( m_source, AL_BUFFER, NULL );

	// Fill all the Buffers with decoded audio data from the OggVorbis file
	m_buffer = new char[m_bufferSize];
	unsigned long bytesWritten = 0;

	bytesWritten = decodeOggVorbis_( &m_oggFile, m_buffer, m_bufferSize );
	if ( bytesWritten )
	{
		alBufferData( m_alID, m_format, m_buffer, bytesWritten, m_frequency );
		alSourceQueueBuffers( m_source, 1, &m_alID );
	}

	bytesWritten = decodeOggVorbis_( &m_oggFile, m_buffer, m_bufferSize );
	if ( bytesWritten )
	{
		alBufferData( m_alID2, m_format, m_buffer, bytesWritten, m_frequency );
		alSourceQueueBuffers( m_source, 1, &m_alID2 );
	}

	// Start playing source
	alSourcePlay( m_source );

	m_updating = true;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundBufferStream::stop()
{
	alSourceStop( m_source );
	alSourcei( m_source, AL_BUFFER, 0 );

	if ( m_buffer )
	{
		delete m_buffer;
		m_buffer = NULL;
	}

	m_updating = false;
}
//////////////////////////////////////////////////////////////////////////
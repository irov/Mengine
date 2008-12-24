/*
 *	OALSoundBufferStream.cpp
 *
 *	Created by _Berserk_ on 19.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "OALSoundBufferStream.h"
#	include "Interface/SoundCodecInterface.h"
#	include "boost/thread.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferStream::OALSoundBufferStream()
		: OALSoundBuffer()
		, m_soundDecoder( NULL )
		, m_alBufferName2( 0 )
		, m_bufferSize( 0 )
		, m_source( 0 )
		, m_looped( false )
		, m_updating( false )
		, m_boostThread( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferStream::~OALSoundBufferStream()
	{
		if( m_boostThread != NULL )
		{
			delete m_boostThread;
			m_boostThread = NULL;
			delete m_boostMutex;
			m_boostMutex = NULL;
		}
		if( m_dataBuffer != NULL )
		{
			delete m_dataBuffer;
			m_dataBuffer = NULL;
		}
		if( m_alBufferName != 0 )
		{
			alDeleteBuffers( 1, &m_alBufferName );
			m_alBufferName = 0;
		}
		if( m_alBufferName2 != 0 )
		{
			alDeleteBuffers( 1, &m_alBufferName2 );
			m_alBufferName2 = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::load( SoundDecoderInterface* _soundDecoder )
	{
		alGenBuffers( 1, &m_alBufferName );
		if( ALenum error = alGetError() != AL_NO_ERROR )
		{
			// TODO: report in case of error
			return false;
		}

		alGenBuffers( 1, &m_alBufferName2 );
		if( ALenum error = alGetError() != AL_NO_ERROR )
		{
			// TODO: report in case of error
			return false;
		}

		m_soundDecoder = _soundDecoder;

		const SoundCodecDataInfo* dataInfo = static_cast<const SoundCodecDataInfo*>( m_soundDecoder->getCodecDataInfo() );
		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_time_total = dataInfo->time_total_secs;

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
	void OALSoundBufferStream::play( ALenum _source, bool _looped, float _pos )
	{
		m_source = _source;
		m_looped = _looped;

		ALint state = 0;
		alGetSourcei( _source, AL_SOURCE_STATE, &state );
		if( state != AL_STOPPED && state != AL_INITIAL )
		{
			alSourceStop( _source );
			//alSourceRewind( _source );
		}

		
		alSourcei( _source, AL_BUFFER, NULL ); // clear source buffering
		alSourcei( _source, AL_LOOPING, AL_FALSE );
		ALenum error = alGetError();
		if( error != AL_NO_ERROR )
		{
		}

		m_dataBuffer = new unsigned char[m_bufferSize];

		m_soundDecoder->seek( _pos );
		unsigned int bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );
		if ( bytesWritten )
		{
			alBufferData( m_alBufferName, m_format, m_dataBuffer, m_bufferSize, m_frequency );
			alSourceQueueBuffers( m_source, 1, &m_alBufferName );
		}

		bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );
		if ( bytesWritten )
		{
			alBufferData( m_alBufferName2, m_format, m_dataBuffer, m_bufferSize, m_frequency );
			alSourceQueueBuffers( m_source, 1, &m_alBufferName2 );
		}

		alSourcePlay( m_source );
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
		}

		m_updating = true;

		m_boostMutex = new boost::mutex;
		m_boostThread = new boost::thread( boost::bind( &OALSoundBufferStream::updateStream_, this ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::pause( ALenum _source )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::stop( ALenum _source )
	{
		setUpdating( false );
		if( m_boostThread )
		{
			m_boostThread->join();
			delete m_boostThread;
			m_boostThread = NULL;
			delete m_boostMutex;
			m_boostMutex = NULL;
		}

		ALint queued = 0;
		ALuint buffer = 0;
		// Получаем количество отработанных буферов
		alGetSourcei( m_source, AL_BUFFERS_PROCESSED, &queued );
		ALenum error = alGetError();
		if( error != AL_NO_ERROR )
		{
			printf( "OAL Error: %s\n", alGetString( error ) );
		}

		// Если таковые существуют то
		while( queued-- )
		{
			// Исключаем их из очереди
			alSourceUnqueueBuffers( m_source, 1, &buffer );
			error = alGetError();
			if( error != AL_NO_ERROR )
			{
				printf( "OAL Error: %s\n", alGetString( error ) );
			}
		}

		alSourceStop( m_source );
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			printf( "OAL Error: %s\n", alGetString( error ) );
		}

		// unqueue remaining buffers
		alGetSourcei( m_source, AL_BUFFERS_QUEUED, &queued );
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			printf( "OAL Error: %s\n", alGetString( error ) );
		}
		while( queued-- )
		{
			alSourceUnqueueBuffers( m_source, 1, &buffer );
			error = alGetError();
			if( error != AL_NO_ERROR )
			{
				printf( "OAL Error: %s\n", alGetString( error ) );
			}
		}
		
		if( m_dataBuffer != NULL )
		{
			delete m_dataBuffer;
			m_dataBuffer = NULL;
		}

		m_soundDecoder->seek( 0.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::setUpdating( bool _updating )
	{
		if( m_boostMutex != NULL )
		{
			boost::mutex::scoped_lock scoped_mutex( *m_boostMutex );
		}
		m_updating = _updating;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::getUpdating()
	{
		if( m_boostMutex != NULL )
		{
			boost::mutex::scoped_lock scoped_mutex( *m_boostMutex );
		}
		return m_updating;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::updateStream_()
	{
		while( getUpdating() == true )
		{
			int processed = 0;
			unsigned int bytesWritten = 0;

			ALuint buffer;

			// Получаем количество отработанных буферов
			alGetSourcei( m_source, AL_BUFFERS_PROCESSED, &processed );

			// Если таковые существуют то
			while( processed-- )
			{
				// Исключаем их из очереди
				alSourceUnqueueBuffers( m_source, 1, &buffer );

				// Читаем очередную порцию данных
				bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );
				if ( bytesWritten )
				{
					// Включаем буфер обратно в очередь
					alBufferData( buffer, m_format, m_dataBuffer, m_bufferSize, m_frequency );
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
			
			Sleep( 1 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundBufferStream::getTimePos( ALenum _source )
	{
		return m_soundDecoder->timeTell();
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
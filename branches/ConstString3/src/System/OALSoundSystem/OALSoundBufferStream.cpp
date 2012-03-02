/*
 *	OALSoundBufferStream.cpp
 *
 *	Created by _Berserk_ on 19.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "OALSoundBufferStream.h"
#	include "OALSoundSystem.h"

#	include "Interface/SoundCodecInterface.h"

#	include "Logger/Logger.h"

#	include "OALError.h"

#	define OAL_CHECK_ERROR() s_OALErrorCheck( m_soundSystem, __FILE__, __LINE__ )

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferStream::OALSoundBufferStream( OALSoundSystem * _soundSystem )
		: OALSoundBuffer(_soundSystem)
		, m_soundDecoder(NULL)
		, m_alBufferId2(0)
		, m_bufferSize(0)
		, m_sourceId(0)
		, m_updating(false)
		, m_dataBuffer(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferStream::~OALSoundBufferStream()
	{
		if( m_alBufferId2 != 0 )
		{
			m_soundSystem->releaseBufferId( m_alBufferId2 );
			m_alBufferId2 = 0;
		}

		if( m_dataBuffer != NULL )
		{
			delete[] m_dataBuffer;
			m_dataBuffer = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::load( SoundDecoderInterface * _soundDecoder )
	{
		m_alBufferId = m_soundSystem->genBufferId();

		if( m_alBufferId == 0 )
		{
			// TODO: report in case of error
			return false;
		}

		m_alBufferId2 = m_soundSystem->genBufferId();
		if( ALenum error = alGetError() != AL_NO_ERROR )
		{
			// TODO: report in case of error
			return false;
		}

		m_soundDecoder = _soundDecoder;

		const SoundCodecDataInfo* dataInfo = m_soundDecoder->getCodecDataInfo();
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

		m_dataBuffer = new unsigned char[m_bufferSize];

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::play( ALenum _source, bool _looped, float _pos )
	{
		m_sourceId = _source;

		ALint state = 0;
		alGetSourcei( m_sourceId, AL_SOURCE_STATE, &state );
		OAL_CHECK_ERROR();
		if( state != AL_STOPPED && state != AL_INITIAL )
		{
			alSourceStop( m_sourceId );
			OAL_CHECK_ERROR();
			//alSourceRewind( _source );
		}
				
		alSourcei( m_sourceId, AL_BUFFER, 0 ); // clear source buffering
		OAL_CHECK_ERROR();
		alSourcei( m_sourceId, AL_LOOPING, AL_FALSE );
		OAL_CHECK_ERROR();

		m_soundDecoder->seek( _pos );
		unsigned int bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );
			
		if ( bytesWritten )
		{
			alBufferData( m_alBufferId, m_format, m_dataBuffer, m_bufferSize, m_frequency );
			OAL_CHECK_ERROR();
			alSourceQueueBuffers( m_sourceId, 1, &m_alBufferId );
			OAL_CHECK_ERROR();
		}

		bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );

		if ( bytesWritten )
		{
			alBufferData( m_alBufferId2, m_format, m_dataBuffer, m_bufferSize, m_frequency );
			OAL_CHECK_ERROR();
			alSourceQueueBuffers( m_sourceId, 1, &m_alBufferId2 );
			OAL_CHECK_ERROR();
		}

		alSourcePlay( m_sourceId );
		OAL_CHECK_ERROR();

		this->setUpdating( true );

		//m_threadID = new pthread_t;
		// there is no need to check errors
		//pthread_create( m_threadID, NULL, &OALSoundBufferStream::s_updateStream_, this );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::pause( ALenum _source )
	{
		this->stop( _source );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::stop( ALenum _source )
	{
		this->setUpdating( false );
		/*if( m_threadID != NULL )
		{
			int res = 0;
			pthread_join( *m_threadID, (void**)&res );
			delete m_threadID;
			m_threadID = NULL;
		}*/

		m_soundSystem->clearSourceId( m_sourceId );
		
		m_soundDecoder->seek( 0.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::setUpdating( bool _updating )
	{
		/*if( m_boostMutex != NULL )
		{
			boost::mutex::scoped_lock scoped_mutex( *m_boostMutex );
		}*/
		m_updating = _updating;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::getUpdating() const
	{
		/*if( m_boostMutex != NULL )
		{
			boost::mutex::scoped_lock scoped_mutex( *m_boostMutex );
		}*/
		return m_updating;
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundBufferStream::getTimePos( ALenum _source )
	{
		return m_soundDecoder->timeTell();
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::update()
	{
		if( this->m_updating == false )
		{
			return;
		}

		int processed = 0;
		unsigned int bytesWritten = 0;

		ALuint buffer;

		alSourcei( m_sourceId, AL_LOOPING, AL_FALSE );

		// Получаем количество отработанных буферов
		alGetSourcei( m_sourceId, AL_BUFFERS_PROCESSED, &processed );

		// Если таковые существуют то
		while( processed-- > 0 )
		{
			// Исключаем их из очереди
			alSourceUnqueueBuffers( m_sourceId, 1, &buffer );

			// Читаем очередную порцию данных
			bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );

			if( bytesWritten == 0 )
			{
				m_soundDecoder->seek( 0.f );

				bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );
			}

			if( bytesWritten == 0 )
			{
				LogSystemInterface * logSystem = m_soundSystem->getLogSystem();

				LOGGER_ERROR(logSystem)("OALSoundBufferStream::update bytesWritten is zero"
					);
			}

			if( bytesWritten )
			{
				// Включаем буфер обратно в очередь
				alBufferData( buffer, m_format, m_dataBuffer, m_bufferSize, m_frequency );
				alSourceQueueBuffers( m_sourceId, 1, &buffer );
			}
		}

		int state;
		alGetSourcei( m_sourceId , AL_SOURCE_STATE, &state );
		if( state != AL_PLAYING )
		{
			alSourcePlay( m_sourceId );
		}

		//// Check the status of the Source.  If it is not playing, then playback was completed,
		//// or the Source was starved of audio data, and needs to be restarted.
		//int state;
		//alGetSourcei( m_source, AL_SOURCE_STATE, &state );
		//if (state != AL_PLAYING)
		//{
		//	// If there are Buffers in the Source Queue then the Source was starved of audio
		//	// data, so needs to be restarted (because there is more audio data to play)
		//	int queuedBuffers;
		//	alGetSourcei( m_source, AL_BUFFERS_QUEUED, &queuedBuffers );
		//	if ( queuedBuffers )
		//	{
		//		alSourcePlay( m_source );
		//	}
		//}	
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

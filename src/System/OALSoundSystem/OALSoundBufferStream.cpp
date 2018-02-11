#	include "OALSoundBufferStream.h"
#	include "OALSoundSystem.h"
#	include "OALSoundError.h"

#	include "Interface/SoundCodecInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferStream::OALSoundBufferStream()
		: m_sourceId( 0 )
		, m_looped( false )
		, m_updating( false )
	{
		for( uint32_t i = 0; i != OPENAL_STREAM_BUFFER_COUNT; ++i )
		{
			m_alBuffersId[i] = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferStream::~OALSoundBufferStream()
	{
		this->removeBuffers_();
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::removeBuffers_()
	{
		for( uint32_t i = 0; i != OPENAL_STREAM_BUFFER_COUNT; ++i )
		{
			ALuint id = m_alBuffersId[i];

			if( id != 0 )
			{
				m_soundSystem->releaseBufferId( id );
			}

			m_alBuffersId[i] = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::load( const SoundDecoderInterfacePtr & _soundDecoder )
	{
		for( uint32_t i = 0; i != OPENAL_STREAM_BUFFER_COUNT; ++i )
		{
			ALuint id = m_soundSystem->genBufferId();

			if( id == 0 )
			{
				// TODO: report in case of error
				LOGGER_ERROR("OALSoundBufferStream::load invalid gen %d buffer ID"
					, i
					);

				this->removeBuffers_();

				return false;
			}

			m_alBuffersId[i] = id;
		}

		m_soundDecoder = _soundDecoder;

		const SoundCodecDataInfo* dataInfo = m_soundDecoder->getCodecDataInfo();
		m_frequency = dataInfo->frequency;

		if( dataInfo->channels != 2 )
		{
			LOGGER_ERROR("OALSoundBufferStream::load invalid channels %d must be %d"
				, dataInfo->channels
				, 2
				);

			return false;
		}

		//m_channels = dataInfo->channels;
		m_channels = 2;
		m_length = dataInfo->length;

		if( m_channels == 1 )
		{
			m_format = AL_FORMAT_MONO16;
			// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
			//m_bufferSize = m_frequency >> 1;
			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			//m_bufferSize -= (m_bufferSize % 2);
			m_isStereo = false;
		}
		else if( m_channels == 2 )
		{
			m_format = AL_FORMAT_STEREO16;
			// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
			//m_bufferSize = m_frequency;
			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			//m_bufferSize -= (m_bufferSize % 4);
			m_isStereo = true;
		}
		else if( m_channels == 4 )
		{
			m_format = alGetEnumValue( "AL_FORMAT_QUAD16" );
			// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
			//m_bufferSize = m_frequency * 2;
			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			//m_bufferSize -= (m_bufferSize % 8);
			m_isStereo = true;
		}
		else if( m_channels == 6 )
		{
			m_format = alGetEnumValue( "AL_FORMAT_51CHN16" );
			// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
			//m_bufferSize = m_frequency * 3;
			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			//m_bufferSize -= (m_bufferSize % 12);
			m_isStereo = true;
		}
		else
		{
			LOGGER_ERROR("OALSoundBufferStream::load invalid channels %d"
				, m_channels
				);

			return false;
		}

		//m_dataBuffer = new unsigned char[m_bufferSize /*+ fixed_sound_buffer_size*/];

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::play( ALuint _source, bool _looped, float _pos )
	{
		m_sourceId = _source;
		m_looped = _looped;

		if( _pos > m_length )
		{
			LOGGER_ERROR("OALSoundBufferStream::play pos %f > length %f"
				, _pos
				, m_length
				);

			return false;
		}

		ALint state = 0;
		alGetSourcei( m_sourceId, AL_SOURCE_STATE, &state );
		OAL_CHECK_ERROR();

		if( state != AL_STOPPED && state != AL_INITIAL )
		{
			this->stop( m_sourceId );
			//alSourceRewind( _source );
		}

		alSourcei( m_sourceId, AL_BUFFER, 0 ); // clear source buffering
		OAL_CHECK_ERROR();

		alSourcei( m_sourceId, AL_LOOPING, AL_FALSE );
		OAL_CHECK_ERROR();

		if( m_soundDecoder->seek( _pos ) == false )
		{
			LOGGER_ERROR("OALSoundBufferStream::play invalid seek '%f'"
				, _pos
				);

			return false;
		}

		for( uint32_t i = 0; i != OPENAL_STREAM_BUFFER_COUNT; ++i )
		{
			ALuint bufferId = m_alBuffersId[i];

			size_t bytesWritten;
			if( this->bufferData_( bufferId, bytesWritten ) == false )
			{
				return false;
			}

			alSourceQueueBuffers( m_sourceId, 1, &bufferId );
			OAL_CHECK_ERROR();
		}

		alSourcePlay( m_sourceId );
		OAL_CHECK_ERROR();

		this->setUpdating_( true );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::resume( ALuint _source )
	{
		(void)_source;

		alSourcePlay( m_sourceId );
		OAL_CHECK_ERROR();

		this->setUpdating_( true );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::pause( ALuint _source )
	{
		this->setUpdating_( false );

		alSourcePause( _source );
		OAL_CHECK_ERROR();
		//m_soundSystem->clearSourceId( _source );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::stop( ALuint _source )
	{
		this->setUpdating_( false );

		ALint process_count = 0;
		// Получаем количество отработанных буферов
		alGetSourcei( _source, AL_BUFFERS_PROCESSED, &process_count );
		OAL_CHECK_ERROR();

		// Если таковые существуют то
		while( process_count-- > 0 )
		{
			// Исключаем их из очереди
			ALuint buffer = 0;

			alSourceUnqueueBuffers( _source, 1, &buffer );
			OAL_CHECK_ERROR();
		}

		alSourceStop( _source );
		OAL_CHECK_ERROR();

		ALint queued_count = 0;
		// unqueue remaining buffers
		alGetSourcei( _source, AL_BUFFERS_QUEUED, &queued_count );
		OAL_CHECK_ERROR();

		while( queued_count-- > 0 )
		{
			// Исключаем их из очереди
			ALuint buffer = 0;

			alSourceUnqueueBuffers( _source, 1, &buffer );
			OAL_CHECK_ERROR();
		}

		alSourcei( m_sourceId, AL_BUFFER, 0 ); // clear source buffering

		alSourceRewind( _source );
		OAL_CHECK_ERROR();
		//m_soundDecoder->seek( 0.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::setUpdating_( bool _updating )
	{
		m_updating = _updating;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::getUpdating_() const
	{
		return m_updating;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::setTimePos( ALuint _source, float _pos ) const
	{
		(void)_source;

		bool result = m_soundDecoder->seek( _pos );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::getTimePos( ALuint _source, float & _pos ) const
	{
		(void)_source;

		float timeTell = m_soundDecoder->tell();

		if( timeTell > m_length )
		{
			LOGGER_ERROR("OALSoundBufferStream::getTimePos get tell %f > length %f"
				, timeTell
				, m_length
				);

			return false;
		}

		_pos = timeTell;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::update()
	{
		if( this->m_updating == false )
		{
			return true;
		}

		//// Check the status of the Source.  If it is not playing, then playback was completed,
		//// or the Source was starved of audio data, and needs to be restarted.
		//alSourcei( m_sourceId, AL_LOOPING, AL_FALSE );
		//      OAL_CHECK_ERROR();

		//int queuedBuffers;
		//alGetSourcei( m_sourceId, AL_BUFFERS_QUEUED, &queuedBuffers );

		ALint processed = 0;
		alGetSourcei( m_sourceId, AL_BUFFERS_PROCESSED, &processed );
		OAL_CHECK_ERROR();

		bool end = false;
		for( ALint curr_processed = 0; curr_processed != processed; ++curr_processed )
		{
			// Исключаем их из очереди
			ALuint bufferId;
			alSourceUnqueueBuffers( m_sourceId, 1, &bufferId );
			OAL_CHECK_ERROR();

			// Читаем очередную порцию данных
			size_t bytesWritten;
			this->bufferData_( bufferId, bytesWritten );

			if( bytesWritten == 0 )
			{
				if( m_looped == true )
				{
					m_soundDecoder->rewind();

					this->bufferData_( bufferId, bytesWritten );
				}
				else
				{
					end = true;

					continue;
				}
			}

			alSourceQueueBuffers( m_sourceId, 1, &bufferId );
			OAL_CHECK_ERROR();
		}

		if( end == true )
		{
			return false;
		}

		ALint state;
		alGetSourcei( m_sourceId, AL_SOURCE_STATE, &state );
		OAL_CHECK_ERROR();

		if( state != AL_PLAYING && state != AL_PAUSED )
		{
			alSourcePlay( m_sourceId );
			OAL_CHECK_ERROR();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::bufferData_( ALuint _alBufferId, size_t & _bytes )
	{
		char dataBuffer[OPENAL_STREAM_BUFFER_SIZE];
		size_t bytesWritten = m_soundDecoder->decode( dataBuffer, OPENAL_STREAM_BUFFER_SIZE );

		_bytes = bytesWritten;

		if( bytesWritten == 0 )
		{
			return true;
		}

		ALsizei al_bytesWritten = (ALsizei)bytesWritten;
		alBufferData( _alBufferId, m_format, dataBuffer, al_bytesWritten, m_frequency );

		if( OAL_CHECK_ERROR() == false )
		{
			LOGGER_ERROR("OALSoundBufferStream::play buffer=%d id=%d format=%d bytes=%d frequency=%d"
				, _alBufferId
				, m_format
				, bytesWritten
				, m_frequency
				);

			return false;
		}

		return true;
	}
}	// namespace Menge

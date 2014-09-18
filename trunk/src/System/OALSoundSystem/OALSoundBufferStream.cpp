#	include "OALSoundBufferStream.h"
#	include "OALSoundSystem.h"

#	include "Interface/SoundCodecInterface.h"

#	include "Logger/Logger.h"

#	include "OALError.h"

#   include <malloc.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferStream::OALSoundBufferStream()
		: m_soundDecoder(nullptr)
		, m_sourceId(0)
		, m_looped(false)
		, m_updating(false)
	{
        for( size_t i = 0; i != OPENAL_STREAM_BUFFER_COUNT; ++i )
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
        for( size_t i = 0; i != OPENAL_STREAM_BUFFER_COUNT; ++i )
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
        for( size_t i = 0; i != OPENAL_STREAM_BUFFER_COUNT; ++i )
        {
            ALuint id = m_soundSystem->genBufferId();

            if( id == 0 )
            {
                // TODO: report in case of error
                LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::load invalid gen %d buffer ID"
                    , i
                    );

                this->removeBuffers_();

                return false;
            }

            m_alBuffersId[i] = id;
        }

		//m_alBufferId2 = m_soundSystem->genBufferId();
		//if( m_alBufferId2 == 0 )
		//{
		//	// TODO: report in case of error
  //          LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::load invalid gen second buffer ID"
  //              );

		//	return false;
		//}

		m_soundDecoder = _soundDecoder;

		const SoundCodecDataInfo* dataInfo = m_soundDecoder->getCodecDataInfo();
		m_frequency = dataInfo->frequency;

        if( dataInfo->channels != 2 )
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::load invalid channels %d must be %d"
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
		else if (m_channels == 2)
		{
			m_format = AL_FORMAT_STEREO16;
			// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
			//m_bufferSize = m_frequency;
			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			//m_bufferSize -= (m_bufferSize % 4);
			m_isStereo = true;
		}
		else if (m_channels == 4)
		{
			m_format = alGetEnumValue("AL_FORMAT_QUAD16");
			// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
			//m_bufferSize = m_frequency * 2;
			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			//m_bufferSize -= (m_bufferSize % 8);
			m_isStereo = true;
		}
		else if (m_channels == 6)
		{
			m_format = alGetEnumValue("AL_FORMAT_51CHN16");
			// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
			//m_bufferSize = m_frequency * 3;
			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			//m_bufferSize -= (m_bufferSize % 12);
			m_isStereo = true;
		}
        else
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::load invalid channels %d"
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
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::play pos %f > length %f"
                , _pos
                , m_length
                );

            return false;
        }

		ALint state = 0;
		alGetSourcei( m_sourceId, AL_SOURCE_STATE, &state );
		OAL_CHECK_ERROR(m_serviceProvider);

		if( state != AL_STOPPED && state != AL_INITIAL )
		{
            this->stop( m_sourceId );
			//alSourceRewind( _source );
		}
				
		alSourcei( m_sourceId, AL_BUFFER, 0 ); // clear source buffering
		OAL_CHECK_ERROR(m_serviceProvider);


		//ALint al_loop = _looped ? AL_TRUE : AL_FALSE;
		//alSourcei( m_sourceId, AL_LOOPING, al_loop );
		//OAL_CHECK_ERROR(m_serviceProvider);

		alSourcei( m_sourceId, AL_LOOPING, AL_FALSE );
		OAL_CHECK_ERROR(m_serviceProvider);

		if( m_soundDecoder->seek( _pos ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::play invalid seek '%f'"
                , _pos
                );

            return false;
        }
        
        for( size_t i = 0; i != OPENAL_STREAM_BUFFER_COUNT; ++i )
        {
            ALuint id = m_alBuffersId[i];

            size_t bytesWritten;
            if( this->bufferData_( id, bytesWritten ) == false )
            {
                return false;
            }
        }
                
		alSourcePlay( m_sourceId );
		OAL_CHECK_ERROR(m_serviceProvider);

		this->setUpdating( true );

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool OALSoundBufferStream::resume( ALuint _source )
    {
		(void)_source;

        alSourcePlay( m_sourceId );
        OAL_CHECK_ERROR(m_serviceProvider);

        this->setUpdating( true );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::pause( ALuint _source )
	{
		this->setUpdating( false );
        
        alSourcePause( _source );
        OAL_CHECK_ERROR(m_serviceProvider);
        //m_soundSystem->clearSourceId( _source );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::stop( ALuint _source )
	{
		this->setUpdating( false );

        ALint process_count = 0;
        // �������� ���������� ������������ �������
        alGetSourcei( _source, AL_BUFFERS_PROCESSED, &process_count );
        OAL_CHECK_ERROR(m_serviceProvider);

        // ���� ������� ���������� ��
        while( process_count-- > 0 )
        {
            // ��������� �� �� �������
            ALuint buffer = 0;

            alSourceUnqueueBuffers( _source, 1, &buffer );
            OAL_CHECK_ERROR(m_serviceProvider);
        }

        alSourceStop( _source );        
        OAL_CHECK_ERROR(m_serviceProvider);

        ALint queued_count = 0;
        // unqueue remaining buffers
        alGetSourcei( _source, AL_BUFFERS_QUEUED, &queued_count );
        OAL_CHECK_ERROR(m_serviceProvider);

        while( queued_count-- > 0 )
        {
            // ��������� �� �� �������
            ALuint buffer = 0;

            alSourceUnqueueBuffers( _source, 1, &buffer );
            OAL_CHECK_ERROR(m_serviceProvider);
        }

		{
			ALint val;

			do 
			{
				alGetSourcei( _source, AL_SOURCE_STATE, &val );
			} 
			while( val == AL_PLAYING );
		}

        alSourcei( m_sourceId, AL_BUFFER, 0 ); // clear source buffering
        
        alSourceRewind( _source );
        OAL_CHECK_ERROR(m_serviceProvider);
		//m_soundDecoder->seek( 0.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::setUpdating( bool _updating )
	{
		m_updating = _updating;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::getUpdating() const
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
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::getTimePos get tell %f > length %f"
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
        ALint state;
        alGetSourcei( m_sourceId, AL_SOURCE_STATE, &state );
        OAL_CHECK_ERROR(m_serviceProvider);

		//alSourcei( m_sourceId, AL_LOOPING, AL_FALSE );
  //      OAL_CHECK_ERROR(m_serviceProvider);

        //int queuedBuffers;
        //alGetSourcei( m_sourceId, AL_BUFFERS_QUEUED, &queuedBuffers );

		// �������� ���������� ������������ �������
        ALint processed = 0;
		alGetSourcei( m_sourceId, AL_BUFFERS_PROCESSED, &processed );
        OAL_CHECK_ERROR(m_serviceProvider);
                       
		bool end = false;
		for( ALint curr_processed = 0; curr_processed != processed; ++curr_processed )
		{
			// ��������� �� �� �������
            ALuint bufferId;
			alSourceUnqueueBuffers( m_sourceId, 1, &bufferId );
            OAL_CHECK_ERROR(m_serviceProvider);
                        
			// ������ ��������� ������ ������
            unsigned int bytesWritten;
			this->bufferData_( bufferId, bytesWritten );

			if( bytesWritten == 0 )
			{
				end = true;
			}
		}

		if( state != AL_PLAYING && state != AL_PAUSED )
		{
			if( m_looped == true )
			{
				m_soundDecoder->rewind();
			
				alSourceStop( m_sourceId );
				OAL_CHECK_ERROR(m_serviceProvider);
			}

			// If there are Buffers in the Source Queue then the Source was starved of audio
			// data, so needs to be restarted (because there is more audio data to play)
			ALint queuedBuffers;
			alGetSourcei( m_sourceId, AL_BUFFERS_QUEUED, &queuedBuffers );
			OAL_CHECK_ERROR(m_serviceProvider);

			if( queuedBuffers == 0 )
			{
				return false;
			}

			alSourcePlay( m_sourceId );
			OAL_CHECK_ERROR(m_serviceProvider);			
		}	

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::rewind()
	{
		bool successful = m_soundDecoder->rewind();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
    bool OALSoundBufferStream::bufferData_( ALuint _alBufferId, size_t & _bytes  )
    {
        char dataBuffer[OPENAL_STREAM_BUFFER_SIZE];

        size_t bytesWritten = m_soundDecoder->decode( dataBuffer, OPENAL_STREAM_BUFFER_SIZE );

		_bytes = bytesWritten;
        
		if( bytesWritten == 0 )
		{
			return true;
		}

        alBufferData( _alBufferId, m_format, dataBuffer, bytesWritten, m_frequency );
        
        if( OAL_CHECK_ERROR(m_serviceProvider) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::play buffer=%d id=%d format=%d bytes=%d frequency=%d"
                , _alBufferId
                , m_format
                , bytesWritten
                , m_frequency
                );

            return false;
        }

        alSourceQueueBuffers( m_sourceId, 1, &_alBufferId );
        OAL_CHECK_ERROR(m_serviceProvider);

        return true;
    }
}	// namespace Menge

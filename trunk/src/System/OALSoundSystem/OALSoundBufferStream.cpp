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

#	define OAL_CHECK_ERROR() s_OALErrorCheck( m_serviceProvider, __FILE__, __LINE__ )

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferStream::OALSoundBufferStream( ServiceProviderInterface * _serviceProvider, OALSoundSystem * _soundSystem )
		: OALSoundBuffer(_serviceProvider, _soundSystem)
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
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::load invalid gen first buffer ID"
                );

			return false;
		}

		m_alBufferId2 = m_soundSystem->genBufferId();
		if( m_alBufferId2 == 0 )
		{
			// TODO: report in case of error
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::load invalid gen second buffer ID"
                );

			return false;
		}

		m_soundDecoder = _soundDecoder;

		const SoundCodecDataInfo* dataInfo = m_soundDecoder->getCodecDataInfo();
		m_frequency = dataInfo->frequency;
		//m_channels = dataInfo->channels;
        m_channels = 2;
		m_time_total = dataInfo->time_total_secs;

		if (m_channels == 1)
		{
			m_format = AL_FORMAT_MONO16;
			// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
			m_bufferSize = m_frequency >> 1;
			// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
			m_bufferSize -= (m_bufferSize % 2);
            m_isStereo = false;
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
        else
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::load invalid channels %d"
                , m_channels
                );

            return false;
        }

		m_dataBuffer = new unsigned char[m_bufferSize /*+ fixed_sound_buffer_size*/];

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::play( ALenum _source, bool _looped, float _pos )
	{
        (void)_looped;

		m_sourceId = _source;

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
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::play invalid seek '%f'"
                , _pos
                );

            return;
        }

		unsigned int bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );
			
		if ( bytesWritten > 0 )
		{
			alBufferData( m_alBufferId, m_format, m_dataBuffer, bytesWritten, m_frequency );
			if( OAL_CHECK_ERROR() == true )
            {
                LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::play buffer=1 id=%d format=%d bytes=%d frequency=%d"
                    , m_alBufferId
                    , m_format
                    , bytesWritten
                    , m_frequency
                    );
            }

			alSourceQueueBuffers( m_sourceId, 1, &m_alBufferId );
			OAL_CHECK_ERROR();            
		}

		bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );

		if ( bytesWritten > 0 )
		{
			alBufferData( m_alBufferId2, m_format, m_dataBuffer, bytesWritten, m_frequency );


            if( OAL_CHECK_ERROR() == true )
            {
                LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::play buffer=2 id=%d format=%d bytes=%d frequency=%d"
                    , m_alBufferId2
                    , m_format
                    , bytesWritten
                    , m_frequency
                    );
            }

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

		m_soundSystem->clearSourceId( _source );
		
		m_soundDecoder->seek( 0.0f );
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
	bool OALSoundBufferStream::getTimePos( ALenum _source, float & _pos ) const
	{
        (void)_source;

        float timeTell = m_soundDecoder->timeTell();

        _pos = timeTell;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::update()
	{
		if( this->m_updating == false )
		{
			return;
		}
        	
		unsigned int bytesWritten = 0;
        bool is_end = false;

		ALuint buffer;

		alSourcei( m_sourceId, AL_LOOPING, AL_FALSE );
        OAL_CHECK_ERROR();

        int queuedBuffers;
        alGetSourcei( m_sourceId, AL_BUFFERS_QUEUED, &queuedBuffers );

		// Получаем количество отработанных буферов
        int processed = 0;
		alGetSourcei( m_sourceId, AL_BUFFERS_PROCESSED, &processed );
        OAL_CHECK_ERROR();
                
		for( int curr_processed = 0; curr_processed != processed; ++curr_processed )
		{
			// Исключаем их из очереди
			alSourceUnqueueBuffers( m_sourceId, 1, &buffer );
            OAL_CHECK_ERROR();

			// Читаем очередную порцию данных
			bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );

			//if( bytesWritten == 0 )
			//{
			//	m_soundDecoder->seek( 0.f );

			//	bytesWritten = m_soundDecoder->decode( m_dataBuffer, m_bufferSize );
			//}

			//if( bytesWritten == 0 )
			//{
			//	LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::update bytesWritten is zero"
			//		);
			//}

			if( bytesWritten > 0 )
			{
				// Включаем буфер обратно в очередь
				alBufferData( buffer, m_format, m_dataBuffer, bytesWritten, m_frequency );
                OAL_CHECK_ERROR();

				alSourceQueueBuffers( m_sourceId, 1, &buffer );
                OAL_CHECK_ERROR();
			}

            if( bytesWritten < m_bufferSize )
            {
                is_end = true;
            }
		}

        //int state;
        //alGetSourcei( m_sourceId, AL_SOURCE_STATE, &state );
        //OAL_CHECK_ERROR();

        //int queuedBuffers1;
        //alGetSourcei( m_sourceId, AL_BUFFERS_QUEUED, &queuedBuffers1 );

        //if( processed == 0 && queuedBuffers == 0 )
        //{
        //    this->stop( m_sourceId );
        //}
        //else
        //{
        //    if( state == AL_STOPPED )
        //    {
        //        int queuedBuffers;
        //        alGetSourcei( m_sourceId, AL_BUFFERS_QUEUED, &queuedBuffers );

        //        if( queuedBuffers != 0 )
        //        {
        //            alSourcePlay( m_sourceId );
        //        }
        //    }
        //}

		//// Check the status of the Source.  If it is not playing, then playback was completed,
		//// or the Source was starved of audio data, and needs to be restarted.
		int state;
		alGetSourcei( m_sourceId, AL_SOURCE_STATE, &state );
		if( state != AL_PLAYING && processed == 0 )
		{
			// If there are Buffers in the Source Queue then the Source was starved of audio
			// data, so needs to be restarted (because there is more audio data to play)
			int queuedBuffers;
			alGetSourcei( m_sourceId, AL_BUFFERS_QUEUED, &queuedBuffers );
			if( queuedBuffers )
			{
				alSourcePlay( m_sourceId );
			}
		}	
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

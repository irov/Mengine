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

#   include <malloc.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferStream::OALSoundBufferStream()
		: m_soundDecoder(nullptr)
		, m_alBufferId2(0)
		, m_bufferSize(0)
		, m_sourceId(0)
		, m_updating(false)
        //, m_dataBuffer(NULL)
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
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::load( const SoundDecoderInterfacePtr & _soundDecoder )
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

        if( dataInfo->channels != 2 )
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::load invalid channels %d mast be %d"
                , dataInfo->channels
                , 2
                );

            return false;
        }

		//m_channels = dataInfo->channels;
        m_channels = 2;
		m_length = dataInfo->length;

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

		//m_dataBuffer = new unsigned char[m_bufferSize /*+ fixed_sound_buffer_size*/];

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferStream::play( ALenum _source, bool _looped, float _pos )
	{
        (void)_looped;

        if( _pos > m_length )
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::play pos %f > length %f"
                , _pos
                , m_length
                );

            return false;
        }

		m_sourceId = _source;

        alSourceRewind( m_sourceId );
        alSourcei( m_sourceId, AL_BUFFER, 0 );

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

		alSourcei( m_sourceId, AL_LOOPING, AL_FALSE );
		OAL_CHECK_ERROR(m_serviceProvider);

		if( m_soundDecoder->seek( _pos ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("OALSoundBufferStream::play invalid seek '%f'"
                , _pos
                );

            return false;
        }
        
        unsigned int bytesWritten;
        if( this->bufferData_( m_alBufferId, bytesWritten ) == false )
        {
            return false;
        }

        if( this->bufferData_( m_alBufferId2, bytesWritten ) == false )
        {
            return false;
        }
                
		alSourcePlay( m_sourceId );
		OAL_CHECK_ERROR(m_serviceProvider);

		this->setUpdating( true );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferStream::pause( ALenum _source )
	{
		this->setUpdating( false );

        m_soundSystem->clearSourceId( _source );
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
	void OALSoundBufferStream::update()
	{
		if( this->m_updating == false )
		{
			return;
		}

        bool is_end = false;

        //// Check the status of the Source.  If it is not playing, then playback was completed,
        //// or the Source was starved of audio data, and needs to be restarted.
        int state;
        alGetSourcei( m_sourceId, AL_SOURCE_STATE, &state );
        OAL_CHECK_ERROR(m_serviceProvider);

		alSourcei( m_sourceId, AL_LOOPING, AL_FALSE );
        OAL_CHECK_ERROR(m_serviceProvider);

        int queuedBuffers;
        alGetSourcei( m_sourceId, AL_BUFFERS_QUEUED, &queuedBuffers );

		// Получаем количество отработанных буферов
        int processed = 0;
		alGetSourcei( m_sourceId, AL_BUFFERS_PROCESSED, &processed );
        OAL_CHECK_ERROR(m_serviceProvider);
                       
		for( int curr_processed = 0; curr_processed != processed; ++curr_processed )
		{
			// Исключаем их из очереди
            ALuint buffer;
			alSourceUnqueueBuffers( m_sourceId, 1, &buffer );
            OAL_CHECK_ERROR(m_serviceProvider);
                        
			// Читаем очередную порцию данных
            unsigned int bytesWritten;
			if( this->bufferData_( buffer, bytesWritten ) == false )
            {
                continue;
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

		if( state != AL_PLAYING && processed == 0 )
		{
			// If there are Buffers in the Source Queue then the Source was starved of audio
			// data, so needs to be restarted (because there is more audio data to play)
			int queuedBuffers;
			alGetSourcei( m_sourceId, AL_BUFFERS_QUEUED, &queuedBuffers );
            OAL_CHECK_ERROR(m_serviceProvider);

			if( queuedBuffers )
			{
				alSourcePlay( m_sourceId );
                OAL_CHECK_ERROR(m_serviceProvider);
			}
		}	
	}
	//////////////////////////////////////////////////////////////////////////
    bool OALSoundBufferStream::bufferData_( ALuint _alBufferId, unsigned int & _bytes  )
    {
        unsigned char * dataBuffer = (unsigned char*)alloca(m_bufferSize);

        unsigned int bytesWritten = m_soundDecoder->decode( dataBuffer, m_bufferSize );

        if( bytesWritten == 0 )
        {
            _bytes = 0;

            return true;
        }

        alBufferData( _alBufferId, m_format, dataBuffer, bytesWritten, m_frequency );
        
        if( OAL_CHECK_ERROR(m_serviceProvider) == true )
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

        _bytes = bytesWritten;

        return true;
    }
}	// namespace Menge

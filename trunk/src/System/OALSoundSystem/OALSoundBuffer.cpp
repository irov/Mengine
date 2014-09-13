#	include "OALSoundBuffer.h"
#	include "OALSoundSystem.h"

#	include "Interface/SoundCodecInterface.h"

#	include "Core/CacheMemoryBuffer.h"

#	include "OALError.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBuffer::OALSoundBuffer()
		: m_serviceProvider(nullptr)
        , m_soundSystem(nullptr)
		, m_alBufferId(0)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundBuffer::~OALSoundBuffer()
	{
		if( m_alBufferId != 0 )
		{
			m_soundSystem->releaseBufferId( m_alBufferId );
			m_alBufferId = 0;
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void OALSoundBuffer::initialize( ServiceProviderInterface * _serviceProvider, OALSoundSystem * _soundSystem )
    {
        m_serviceProvider = _serviceProvider;
        m_soundSystem = _soundSystem;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OALSoundBuffer::update()
    {
        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBuffer::rewind()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBuffer::load( const SoundDecoderInterfacePtr & _soundDecoder )
	{
		m_alBufferId = m_soundSystem->genBufferId();

		if( m_alBufferId == 0 )
		{
			return false;
		}

		const SoundCodecDataInfo* dataInfo = _soundDecoder->getCodecDataInfo();

		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_length = dataInfo->length;
		size_t size = dataInfo->size;
		
		CacheMemoryBuffer binary_buffer(m_serviceProvider, size, "OALSoundBuffer");
		void * binary_memory = binary_buffer.getMemory();

		size_t decode_size = _soundDecoder->decode( binary_memory, size );
        decode_size -= decode_size % 4;

		if( m_channels == 1 )
		{
			m_format = AL_FORMAT_MONO16;
			m_isStereo = false;
		}
		else
		{
			m_format = AL_FORMAT_STEREO16;
			m_isStereo = true;
		}

		alBufferData( m_alBufferId, m_format, binary_memory, decode_size, m_frequency );
		OAL_CHECK_ERROR(m_serviceProvider);
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBuffer::play( ALenum _source, bool _looped, float _pos )
	{
        ALint state = 0;
        alGetSourcei( _source, AL_SOURCE_STATE, &state );
        OAL_CHECK_ERROR(m_serviceProvider);

        if( state == AL_PLAYING )
        {
            //alSourceStop( _source );
            alSourceRewind( _source );
            OAL_CHECK_ERROR(m_serviceProvider);
        }
        
		alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		OAL_CHECK_ERROR(m_serviceProvider);

		alSourcei( _source, AL_LOOPING, _looped ? AL_TRUE : AL_FALSE );
		OAL_CHECK_ERROR(m_serviceProvider);

        alSourcei( _source, AL_BUFFER, m_alBufferId );
        OAL_CHECK_ERROR(m_serviceProvider);

        float al_pos = _pos * 0.001f;
		alSourcef( _source, AL_SEC_OFFSET, al_pos );
		OAL_CHECK_ERROR(m_serviceProvider);

		alSourcePlay( _source );
		OAL_CHECK_ERROR(m_serviceProvider);

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool OALSoundBuffer::resume( ALenum _source )
    {
        alSourcePlay( _source );
        OAL_CHECK_ERROR(m_serviceProvider);

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::pause( ALenum _source )
	{
		alSourcePause( _source );
		OAL_CHECK_ERROR(m_serviceProvider);

		//alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		//OAL_CHECK_ERROR(m_serviceProvider);
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::stop( ALenum _source )
	{		
		alSourceStop( _source );
		OAL_CHECK_ERROR(m_serviceProvider);
		
		{
			ALint val;

			do 
			{
				alGetSourcei( _source, AL_SOURCE_STATE, &val );
			} 
			while( val == AL_PLAYING );
		}

		alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		OAL_CHECK_ERROR(m_serviceProvider);

        alSourceRewind( _source );
		OAL_CHECK_ERROR(m_serviceProvider);	
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBuffer::setTimePos( ALenum _source, float _pos ) const
	{
		float al_pos = _pos * 0.001f;
		alSourcef( _source, AL_SEC_OFFSET, al_pos );
		
		if( OAL_CHECK_ERROR(m_serviceProvider) == true )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBuffer::getTimePos( ALenum _source, float & _pos ) const
	{
		float al_pos = 0.f;

		alGetSourcef( _source, AL_SEC_OFFSET, &al_pos );
		
        if( OAL_CHECK_ERROR(m_serviceProvider) == true )
        {
            return false;
        }

        float pos = al_pos * 1000.f;

        float total = this->getTimeTotal();

        if( pos > total )
        {
            pos = total;
        }

        _pos = pos;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

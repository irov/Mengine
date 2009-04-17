/*
 *	OALSoundBuffer.cpp
 *
 *	Created by _Berserk_ on 17.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "OALSoundBuffer.h"
#	include "Interface/SoundCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBuffer::OALSoundBuffer()
		: OALSoundBufferBase()
		, m_alBufferName( 0 )
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundBuffer::~OALSoundBuffer()
	{
		cleanup_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBuffer::load( SoundDecoderInterface* _soundDecoder )
	{
		cleanup_();

		while( alGetError() != AL_NO_ERROR );
		
		alGenBuffers( 1, &m_alBufferName );
		ALenum error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO: report in case of error
			//printf( "Error: %s\n", alGetString( error ) );
			return false;
		}

		const SoundCodecDataInfo* dataInfo = static_cast<const SoundCodecDataInfo*>( _soundDecoder->getCodecDataInfo() );
		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_time_total = dataInfo->time_total_secs;
		std::size_t size = dataInfo->size;

		unsigned char* buffer = new unsigned char[ size ];
		_soundDecoder->decode( buffer, size );

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
		alBufferData( m_alBufferName, m_format, buffer, size, m_frequency );

		delete[] buffer;
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO: report in case of error
			printf( "Error: %s\n", alGetString( error ) );
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::cleanup_()
	{
		if( m_alBufferName != 0 )
		{
			alDeleteBuffers( 1, &m_alBufferName );
			ALenum error = alGetError();
			if( error != AL_NO_ERROR )
			{
				// TODO: report in case of error
				printf( "Error: %s\n", alGetString( error ) );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::play( ALenum _source, bool _looped, float _pos )
	{
		ALint state = 0;
		alGetSourcei( _source, AL_SOURCE_STATE, &state );
		if( state != AL_STOPPED || state != AL_INITIAL )
		{
			alSourceStop( _source );
		}

		alSourcei( _source, AL_BUFFER, NULL ); // clear source buffering
		ALenum error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "Error: %s\n", alGetString( error ) );
		}

		alSourcei( _source, AL_LOOPING, _looped ? AL_TRUE : AL_FALSE );
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "Error: %s\n", alGetString( error ) );
		}

		alSourcei( _source, AL_BUFFER, m_alBufferName );
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "Error: %s\n", alGetString( error ) );
		}

		alSourcef( _source, AL_SEC_OFFSET, _pos );
		error = alGetError();

		alSourcePlay( _source );
		error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "Error: %s\n", alGetString( error ) );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::pause( ALenum _source )
	{
		alSourcePause( _source );
		ALenum error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "Error: %s\n", alGetString( error ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::stop( ALenum _source )
	{
		alSourceStop( _source );
		ALenum error = alGetError();
		if( error != AL_NO_ERROR )
		{
			// TODO error reporting
			printf( "Error: %s\n", alGetString( error ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundBuffer::getTimePos( ALenum _source )
	{
		float pos = 0.0f;
		alGetSourcef( _source, AL_SEC_OFFSET, &pos );
		ALenum error = alGetError();
		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

/*
 *	OALSoundBuffer.cpp
 *
 *	Created by _Berserk_ on 17.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "OALSoundBuffer.h"
#	include "Interface/SoundCodecInterface.h"

#	include "OALError.h"

#	define OAL_CHECK_ERROR() s_OALErrorCheck( m_soundSystem, __FILE__, __LINE__ )

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBuffer::OALSoundBuffer( OALSoundSystem * _soundSystem )
		: m_soundSystem(_soundSystem)
		, m_alBufferName( 0 )
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundBuffer::~OALSoundBuffer()
	{
		cleanup_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBuffer::load( SoundDecoderInterface * _soundDecoder )
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

		const SoundCodecDataInfo* dataInfo = _soundDecoder->getCodecDataInfo();

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
			//printf( "Error: %s\n", alGetString( error ) );
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
			OAL_CHECK_ERROR();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::play( ALenum _source, bool _looped, float _pos )
	{
		ALint state = 0;
		alGetSourcei( _source, AL_SOURCE_STATE, &state );
		OAL_CHECK_ERROR();
		if( state == AL_PLAYING )
		{
			alSourceStop( _source );
			OAL_CHECK_ERROR();
		}

		alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		OAL_CHECK_ERROR();

		alSourcei( _source, AL_LOOPING, _looped ? AL_TRUE : AL_FALSE );
		OAL_CHECK_ERROR();

		alSourcei( _source, AL_BUFFER, m_alBufferName );
		OAL_CHECK_ERROR();

		alSourcef( _source, AL_SEC_OFFSET, _pos );
		OAL_CHECK_ERROR();

		alSourcePlay( _source );
		OAL_CHECK_ERROR();

	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::pause( ALenum _source )
	{
		alSourcePause( _source );
		OAL_CHECK_ERROR();
		alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		OAL_CHECK_ERROR();
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBuffer::stop( ALenum _source )
	{
		alSourceStop( _source );
		OAL_CHECK_ERROR();
		alSourcei( _source, AL_BUFFER, 0 ); // clear source buffering
		OAL_CHECK_ERROR();
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundBuffer::getTimePos( ALenum _source )
	{
		float pos = 0.0f;
		alGetSourcef( _source, AL_SEC_OFFSET, &pos );
		OAL_CHECK_ERROR();		
		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

/*
 *	OALSoundBufferBase.cpp
 *
 *	Created by _Berserk_ on 24.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "OALSoundBufferBase.h"
#	include "Interface/SoundCodecInterface.h"

#	define OAL_CHECK_ERROR() s_OALErrorCheck( m_soundSystem, __FILE__, __LINE__ )

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferBase::OALSoundBufferBase()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundBufferBase::~OALSoundBufferBase()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::release()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferBase::load( SoundDecoderInterface * _soundDecoder )
	{
		// determine data parameters
		const SoundCodecDataInfo * dataInfo = _soundDecoder->getCodecDataInfo();

		m_frequency = dataInfo->frequency;
		m_channels = dataInfo->channels;
		m_time_total = dataInfo->time_total_secs;
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

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::play( ALenum _source, bool _looped, float _pos )
	{
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::pause( ALenum _source )
	{
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::stop( ALenum _source )
	{
		// nothing to do
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundBufferBase::getTimePos( ALenum _source )
	{
		return 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundBufferBase::isStereo() const
	{
		return m_isStereo;
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundBufferBase::getTimeTotal() const
	{
		return m_time_total;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundBufferBase::update()
	{
		// do nothing
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

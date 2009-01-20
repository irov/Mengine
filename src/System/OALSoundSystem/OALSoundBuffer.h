/*
 *	OALSoundBuffer.h
 *
 *	Created by _Berserk_ on 17.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "OALSoundBufferBase.h"

namespace Menge
{
	class OALSoundBuffer
		: public OALSoundBufferBase
	{
	public:
		OALSoundBuffer();
		virtual ~OALSoundBuffer();

	public:
		virtual bool load( SoundDecoderInterface* _soundDecoder );

		virtual void play( ALenum _source, bool _looped, float _pos );
		virtual	void pause( ALenum _source );
		virtual void stop( ALenum _source );
		virtual float getTimePos( ALenum _source );

	protected:
		ALuint m_alBufferName;

	private:
		void cleanup_();

	};
}	// namespace Menge

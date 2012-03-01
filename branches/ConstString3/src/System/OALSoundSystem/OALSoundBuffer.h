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
	class OALSoundSystem;

	class OALSoundBuffer
		: public OALSoundBufferBase
	{
	public:
		OALSoundBuffer( OALSoundSystem * _soundSystem );
		~OALSoundBuffer();

	public:
		bool load( SoundDecoderInterface * _soundDecoder ) override;

		void play( ALenum _source, bool _looped, float _pos ) override;
		void pause( ALenum _source ) override;
		void stop( ALenum _source ) override;
		float getTimePos( ALenum _source ) override;

	protected:
		OALSoundSystem * m_soundSystem;

		ALuint m_alBufferId;
	};
}	// namespace Menge

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
		bool load( SoundDecoderInterface* _soundDecoder ) override;

		void play( ALenum _source, bool _looped, float _pos ) override;
		void pause( ALenum _source ) override;
		void stop( ALenum _source ) override;
		float getTimePos( ALenum _source ) override;

	protected:
		ALuint m_alBufferName;

	private:
		void cleanup_();

	};
}	// namespace Menge

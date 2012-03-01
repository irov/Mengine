/*
 *	OALSoundBufferStream.h
 *
 *	Created by _Berserk_ on 19.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "OALSoundBuffer.h"

namespace Menge
{
	class OALSoundBufferStream
		: public OALSoundBuffer
	{
	public:
		OALSoundBufferStream( OALSoundSystem * _soundSystem );
		~OALSoundBufferStream();

	public:
		bool load( SoundDecoderInterface * _soundDecoder ) override;

	public:
		void play( ALenum _source, bool _looped, float _pos ) override;
		void pause( ALenum _source ) override;
		void stop( ALenum _source ) override;
		float getTimePos( ALenum _source ) override;

	public:
		void update() override;

	private:
		SoundDecoderInterface* m_soundDecoder;
		ALuint m_alBufferId2;
		size_t m_bufferSize;

		ALuint m_source;
		bool m_loop;
		unsigned char* m_dataBuffer;

		bool m_updating;

	private:
		void setUpdating( bool _updating );
		bool getUpdating() const;
	};
	
}	// namespace Menge

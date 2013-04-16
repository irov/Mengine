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
		OALSoundBuffer( ServiceProviderInterface * _serviceProvider, OALSoundSystem * _soundSystem );
		~OALSoundBuffer();

	public:
		bool load( SoundDecoderInterface * _soundDecoder ) override;

		void play( ALenum _source, bool _looped, float _pos ) override;
		void pause( ALenum _source ) override;
		void stop( ALenum _source ) override;
		bool getTimePos( ALenum _source, float & _pos ) const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
		OALSoundSystem * m_soundSystem;

		ALuint m_alBufferId;
	};
}	// namespace Menge

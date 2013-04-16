/*
 *	OALSoundBufferBase.h
 *
 *	Created by _Berserk_ on 24.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/SoundSystemInterface.h"
#ifdef __APPLE__
#   include <OpenAL/al.h>
#else
#	include <AL/al.h>
#endif

namespace Menge
{
	class OALSoundBufferBase
		: public SoundBufferInterface
	{
	public:
		OALSoundBufferBase();
		virtual ~OALSoundBufferBase();

	public:
		void release() override;
		void update() override;

	public:
		virtual bool load( SoundDecoderInterface * _soundDecoder );

		virtual void play( ALenum _source, bool _looped, float _pos );
		virtual	void pause( ALenum _source );
		virtual void stop( ALenum _source );
		virtual bool getTimePos( ALenum _source, float & _pos ) const;

		bool isStereo() const;
		float getTimeTotal() const;

	protected:
        ALenum m_format;

		int m_frequency;
		int m_channels;
		float m_time_total;
		bool m_isStereo;		
	};
}	// namespace Menge

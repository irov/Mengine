/*
 *	OALSoundBufferStream.h
 *
 *	Created by _Berserk_ on 19.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "OALSoundBuffer.h"

#if defined(WIN32)
#	define PTW32_STATIC_LIB
#endif
#	include <pthread.h>

namespace Menge
{
	class OALSoundBufferStream
		: public OALSoundBuffer
	{
	public:
		OALSoundBufferStream();
		~OALSoundBufferStream();

	public:
		bool load( SoundDecoderInterface* _soundDecoder );

		virtual void play( ALenum _source, bool _looped, float _pos );
		virtual	void pause( ALenum _source );
		virtual void stop( ALenum _source );
		virtual float getTimePos( ALenum _source );

	public:
		void updateStream_();

	private:
		SoundDecoderInterface* m_soundDecoder;
		ALuint m_alBufferName2;
		std::size_t m_bufferSize;

		ALuint m_source;
		bool m_looped;
		unsigned char* m_dataBuffer;

		bool m_updating;
		void setUpdating( bool _updating );
		bool getUpdating();

		pthread_t* m_threadID;
		static void* s_updateStream_( void* _this );
	};
	
}	// namespace Menge

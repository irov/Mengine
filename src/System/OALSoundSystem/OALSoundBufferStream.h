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
		OALSoundBufferStream();
		~OALSoundBufferStream();

	public:
		bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

	public:
		bool play( ALenum _source, bool _looped, float _pos ) override;
		void pause( ALenum _source ) override;
		void stop( ALenum _source ) override;
		bool getTimePos( ALenum _source, float & _pos ) const override;

	public:
		void update() override;

    protected:
        bool bufferData_( ALuint _alBufferId, unsigned int & _bytes );
		
	private:
		SoundDecoderInterfacePtr m_soundDecoder;
		ALuint m_alBufferId2;
		size_t m_bufferSize;

		ALuint m_sourceId;
		bool m_updating;

	private:
		void setUpdating( bool _updating );
		bool getUpdating() const;
	};
	
}	// namespace Menge

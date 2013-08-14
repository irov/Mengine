#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "al/al.h"

namespace Menge
{
	class OALSoundBufferBase
		: public SoundBufferInterface
	{
	public:
		OALSoundBufferBase();
		virtual ~OALSoundBufferBase();
       
	public:
		virtual bool load( const SoundDecoderInterfacePtr & _soundDecoder ) = 0;

		virtual bool play( ALenum _source, bool _looped, float _pos ) = 0;
        virtual bool resume( ALenum _source ) = 0;
		virtual	void pause( ALenum _source ) = 0;
		virtual void stop( ALenum _source ) = 0;
		virtual bool getTimePos( ALenum _source, float & _pos ) const = 0;

		bool isStereo() const;
		float getTimeTotal() const;

	protected:
        ALenum m_format;

		int m_frequency;
		int m_channels;
		float m_length;
		bool m_isStereo;		
	};
}	// namespace Menge

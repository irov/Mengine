#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include <al.h>

namespace Menge
{
	class OALSoundBufferBase
		: public SoundBufferInterface
	{
	public:
		OALSoundBufferBase();
		virtual ~OALSoundBufferBase();

	public:
		void update() override;
        
	public:
		virtual bool load( const SoundDecoderInterfacePtr & _soundDecoder );

		virtual bool play( ALenum _source, bool _looped, float _pos );
		virtual	void pause( ALenum _source );
		virtual void stop( ALenum _source );
		virtual bool getTimePos( ALenum _source, float & _pos ) const;

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

#	pragma once

#	include "OALSoundBufferBase.h"

namespace Menge
{
	class OALSoundBufferMemory
		: public OALSoundBufferBase
	{
	public:
		OALSoundBufferMemory();
		~OALSoundBufferMemory();
        
    public:
        bool update() override;

	public:
		bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

		bool play( ALuint _source, bool _looped, float _pos ) override;
        bool resume( ALuint _source ) override;
		void pause( ALuint _source ) override;
		void stop( ALuint _source ) override;

	public:
		bool setTimePos( ALuint _source, float _pos ) const override;
		bool getTimePos( ALuint _source, float & _pos ) const override;

	protected:
		ALuint m_alBufferId;
	};
}	// namespace Menge

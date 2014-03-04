#	pragma once

#	include "OALSoundBufferBase.h"

namespace Menge
{
	class OALSoundSystem;

	class OALSoundBuffer
		: public OALSoundBufferBase
	{
	public:
		OALSoundBuffer();
		~OALSoundBuffer();
        
    public:
        void initialize( ServiceProviderInterface * _serviceProvider, OALSoundSystem * _soundSystem );

    public:
        bool update() override;

	public:
		bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

		bool play( ALenum _source, bool _looped, float _pos ) override;
        bool resume( ALenum _source ) override;
		void pause( ALenum _source ) override;
		void stop( ALenum _source ) override;

	public:
		bool setTimePos( ALenum _source, float _pos ) const override;
		bool getTimePos( ALenum _source, float & _pos ) const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
		OALSoundSystem * m_soundSystem;

		ALuint m_alBufferId;
	};
}	// namespace Menge

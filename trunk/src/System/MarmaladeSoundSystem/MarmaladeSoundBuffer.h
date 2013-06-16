#	pragma once

#	include "MarmaladeSoundBufferBase.h"

namespace Menge
{
	class MarmaladeSoundSystem;

	class MarmaladeSoundBuffer
		: public MarmaladeSoundBufferBase
	{
	public:
		MarmaladeSoundBuffer();
		~MarmaladeSoundBuffer();

    public:
        void initialize( ServiceProviderInterface * _serviceProvider, MarmaladeSoundSystem * _soundSystem );

	public:
		bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

		void play( ALenum _source, bool _looped, float _pos ) override;
		void pause( ALenum _source ) override;
		void stop( ALenum _source ) override;
		bool getTimePos( ALenum _source, float & _pos ) const override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
		MarmaladeSoundSystem * m_soundSystem;

		ALuint m_alBufferId;
	};
}	// namespace Menge

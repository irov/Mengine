#   pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "OALSoundBufferBase.h"

#	include <al.h>

namespace Menge
{
	class OALSoundSystem;

	class OALSoundSource
		: public SoundSourceInterface
	{
	public:
		OALSoundSource();
		~OALSoundSource();
        
    public:
        void initialize( ServiceProviderInterface * _serviceProvider, OALSoundSystem* _soundSystem );

	public:
		bool play() override;
		void pause() override;
		void stop() override;
		bool isPlaying() const override;

		void setVolume( float _volume ) override;
		float getVolume() const override;

		void setPosition( const mt::vec3f & _pos ) override;
		const mt::vec3f & getPosition() const override;

		void setLoop( bool _loop ) override;
		bool getLoop() const override;

		float getLengthMs() const override;
		float getPosMs() const override;
		bool setPosMs( float _posMs ) override;

		void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) override;
		SoundBufferInterfacePtr getSoundBuffer() const override;

	public:
		void setHeadMode( bool _headMode );
		bool getHeadMode() const;

    protected:
        void unloadBuffer_();
        void releaseSourceId_();

    private:
        ServiceProviderInterface * m_serviceProvider;
        
        OALSoundSystem * m_soundSystem;

        mt::vec3f m_position;
		float m_volume;
		void apply_( ALuint _source );

		ALuint m_sourceId;
		float m_timing;

		OALSoundBufferBasePtr m_soundBuffer;

        bool m_headMode;
        bool m_playing;
        bool m_pausing;
        bool m_loop;
	};
}	// namespace Menge

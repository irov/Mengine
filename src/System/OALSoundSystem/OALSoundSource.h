/*
 *	OALSoundSource.h
 *
 *	Created by _Berserk_ on 17.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "Interface/SoundSystemInterface.h"
#ifdef __APPLE__
#   include <OpenAL/al.h>
#else
#	include <AL/al.h>
#endif

namespace Menge
{
	class OALSoundSystem;
	class OALSoundBufferBase;

	class OALSoundSource
		: public SoundSourceInterface
	{
	public:
		OALSoundSource( ServiceProviderInterface * _serviceProvider, OALSoundSystem* _soundSystem );
		~OALSoundSource();

	public:
		void play() override;
		void pause() override;
		void stop() override;
		bool isPlaying() const override;

		void setVolume( float _volume ) override;
		float getVolume() const override;

		void setPosition( float _x, float _y, float _z ) override;
		const float * getPosition() const override;

		void setLoop( bool _loop ) override;
		bool getLoop() const override;

		float getLengthMs() const override;
		float getPosMs() const override;
		void setPosMs( float _posMs ) override;

		void loadBuffer( SoundBufferInterface* _soundBuffer ) override;
		SoundBufferInterface* getSoundBuffer() const override;

	public:
		void setHeadMode( bool _headMode );
		bool getHeadMode() const;

    protected:
        void unloadBuffer_();

    private:
        ServiceProviderInterface * m_serviceProvider;
        
        OALSoundSystem* m_soundSystem;

		float m_position[3];
		float m_volume;
		void apply_( ALuint _source );

		ALuint m_sourceId;
		float m_timing;

		OALSoundBufferBase* m_soundBuffer;

        bool m_headMode;
        bool m_playing;
        bool m_loop;
	};
}	// namespace Menge

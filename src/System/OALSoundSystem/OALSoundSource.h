/*
 *	OALSoundSource.h
 *
 *	Created by _Berserk_ on 17.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	include "Interface/SoundSystemInterface.h"
#	include <AL/al.h>

namespace Menge
{
	class OALSoundSystem;
	class OALSoundBufferBase;

	class OALSoundSource
		: public SoundSourceInterface
	{
	public:
		OALSoundSource( OALSoundSystem* _soundSystem );
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

		void setLooped( bool _loop ) override;
		bool isLooped() const override;

		float getLengthMs() override;
		float getPosMs() override;
		void setPosMs( float _posMs ) override;

		void loadBuffer( SoundBufferInterface* _soundBuffer ) override;

	public:
		void setHeadMode( bool _headMode );
		bool getHeadMode() const;

	private:
		bool m_headMode;
		bool m_playing;
		float m_position[3];
		float m_volume;
		bool m_looped;
		void apply_( ALuint _source );

		ALuint m_alSourceName;
		float m_timing;

		OALSoundBufferBase* m_soundBuffer;
		
		OALSoundSystem* m_soundSystem;

		void unloadBuffer_();

	};
}	// namespace Menge
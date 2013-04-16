#	pragma once

#	include "Interface/SoundSystemInterface.h"

namespace Menge
{
	class SilentSoundBuffer
		: public SoundBufferInterface
	{
	public:
		SilentSoundBuffer();
		~SilentSoundBuffer();

    public:
        void release() override;
        void update() override;

	public:
		bool load( SoundDecoderInterface * _soundDecoder );

		void play( size_t _id, bool _looped, float _pos );
		void pause( size_t _id );
		void stop( size_t _id );

    public:
		float getTimePos( size_t _id ) const;
        float getTimeTotal() const;

    public:
        float m_playTime;
        float m_pauseTime;

        int m_frequency;
        int m_channels;
        float m_time_total;
        bool m_isStereo;        
	};
}

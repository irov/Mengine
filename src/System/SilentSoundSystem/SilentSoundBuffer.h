#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Core/ServantBase.h"

namespace Mengine
{
	class SilentSoundBuffer
		: public ServantBase<SoundBufferInterface>
	{
	public:
		SilentSoundBuffer();
		~SilentSoundBuffer() override;

    public:        
        bool update() override;

	public:
		const SoundDecoderInterfacePtr & getDecoder() const override;

	public:
		bool load( const SoundDecoderInterfacePtr & _soundDecoder );

		void play( uint32_t _id, bool _looped, float _pos );
		bool resume( uint32_t _id );
		void pause( uint32_t _id );
		void stop( uint32_t _id );

    public:
		float getTimePos( uint32_t _id ) const;
        float getTimeTotal() const;

    public:
		SoundDecoderInterfacePtr m_soundDecoder;

        float m_playTime;
        float m_pauseTime;

        int m_frequency;
        int m_channels;
        float m_length;
        bool m_isStereo;        
	};

	typedef stdex::intrusive_ptr<SilentSoundBuffer> SilentSoundBufferPtr;
}

#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Time.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SilentSoundBuffer
        : public SoundBufferInterface
    {
        DECLARE_FACTORABLE( SilentSoundBuffer );

    public:
        SilentSoundBuffer();
        ~SilentSoundBuffer() override;

    public:
        bool acquireSoundBuffer() override;
        void releaseSoundBuffer() override;

    public:
        bool updateSoundBuffer() override;

    public:
        const SoundDecoderInterfacePtr & getDecoder() const override;

    public:
        bool load( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable );

        void play( uint32_t _id, bool _looped, float _pos );
        bool resume( uint32_t _id );
        void pause( uint32_t _id );
        void stop( uint32_t _id );

    public:
        float getTimePos( uint32_t _id ) const;
        float getTimeTotal() const;

    public:
        SoundDecoderInterfacePtr m_soundDecoder;

        TimeMilliseconds m_playTime;
        TimeMilliseconds m_pauseTime;

        int32_t m_frequency;
        int32_t m_channels;
        float m_length;
        bool m_isStereo;

        bool m_streamable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SilentSoundBuffer, SoundBufferInterface> SilentSoundBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}

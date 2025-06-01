#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Timestamp.h"
#include "Config/UniqueId.h"

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

    public:
        void play( UniqueId _id, bool _looped, float _position );
        bool resume( UniqueId _id );
        void pause( UniqueId _id );
        void stop( UniqueId _id );

    public:
        float getTimePosition( UniqueId _id ) const;
        float getTimeDuration() const;

    public:
        SoundDecoderInterfacePtr m_soundDecoder;

        Timestamp m_playTime;
        Timestamp m_pauseTime;

        int32_t m_frequency;
        int32_t m_channels;
        float m_duration;
        bool m_isStereo;

        bool m_streamable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SilentSoundBuffer, SoundBufferInterface> SilentSoundBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}

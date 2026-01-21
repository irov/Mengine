#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class AndroidSoundBuffer
        : public SoundBufferInterface
    {
        DECLARE_FACTORABLE( AndroidSoundBuffer );

    public:
        AndroidSoundBuffer();
        ~AndroidSoundBuffer() override;

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
        const uint8_t * getData() const;
        size_t getDataSize() const;
        uint32_t getFrequency() const;
        uint32_t getChannels() const;
        uint32_t getBits() const;
        float getDuration() const;
        bool isStreamable() const;

    protected:
        SoundDecoderInterfacePtr m_soundDecoder;
        Vector<uint8_t> m_pcmData;

        uint32_t m_frequency;
        uint32_t m_channels;
        uint32_t m_bits;
        float m_duration;
        bool m_streamable;
    };

    typedef IntrusivePtr<AndroidSoundBuffer, SoundBufferInterface> AndroidSoundBufferPtr;
}


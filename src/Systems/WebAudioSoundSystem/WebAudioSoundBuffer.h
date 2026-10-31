#pragma once

#include "Interface/SoundBufferInterface.h"
#include "Interface/SoundCodecInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class WebAudioSoundBuffer
        : public SoundBufferInterface
    {
        DECLARE_FACTORABLE( WebAudioSoundBuffer );

    public:
        WebAudioSoundBuffer();
        ~WebAudioSoundBuffer() override;

    public:
        uint32_t getBufferId() const;
        float getDuration() const;

    public:
        bool load( const SoundDecoderInterfacePtr & _decoder, bool _streamable );

    public:
        bool acquireSoundBuffer() override;
        void releaseSoundBuffer() override;
        bool updateSoundBuffer() override;
        const SoundDecoderInterfacePtr & getDecoder() const override;

    protected:
        uint32_t createBuffer_( const SoundDecoderInterfacePtr & _decoder );
        uint32_t createStream_( const SoundDecoderInterfacePtr & _decoder );

    protected:
        SoundDecoderInterfacePtr m_decoder;
        uint32_t m_bufferId;
        float m_duration;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<WebAudioSoundBuffer, SoundBufferInterface> WebAudioSoundBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}

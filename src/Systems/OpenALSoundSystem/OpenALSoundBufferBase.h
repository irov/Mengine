#pragma once

#include "Interface/SoundSystemInterface.h"

#include "OpenALSoundIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ReferenceCounter.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenALSoundSystem;
    //////////////////////////////////////////////////////////////////////////
    class OpenALSoundBufferBase
        : public SoundBufferInterface
    {
    public:
        OpenALSoundBufferBase();
        ~OpenALSoundBufferBase() override;

    public:
        bool acquireSoundBuffer() override;
        void releaseSoundBuffer() override;

    public:
        virtual bool _acquireSoundBuffer();
        virtual void _releaseSoundBuffer();

    public:
        const SoundDecoderInterfacePtr & getDecoder() const override;

    public:
        virtual bool load( const SoundDecoderInterfacePtr & _soundDecoder ) = 0;

        virtual bool playSource( ALuint _source, bool _looped, float _pos ) = 0;
        virtual bool resumeSource( ALuint _source ) = 0;
        virtual void pauseSource( ALuint _source ) = 0;
        virtual void stopSource( ALuint _source ) = 0;

    public:
        virtual bool setTimePos( ALuint _source, float _pos ) const = 0;
        virtual bool getTimePos( ALuint _source, float * const _pos ) const = 0;

    public:
        bool isStereo() const;
        float getTimeTotal() const;

    protected:
        ALuint genBufferId();
        void releaseBufferId( ALuint _bufferId );

    protected:
        ReferenceCounter m_refacquire;

        SoundDecoderInterfacePtr m_soundDecoder;

        ALenum m_format;

        uint32_t m_frequency;
        uint32_t m_channels;
        float m_length;
        bool m_isStereo;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenALSoundBufferBase, SoundBufferInterface> OpenALSoundBufferBasePtr;
    //////////////////////////////////////////////////////////////////////////
}

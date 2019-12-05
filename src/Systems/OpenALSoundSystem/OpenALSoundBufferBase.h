#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Kernel/Factorable.h"

#include "AL/al.h"

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
        void setSoundSystem( OpenALSoundSystem * _soundSystem );
        OpenALSoundSystem * getSoundSystem() const;

    public:
        bool acquire() override;
        void release() override;

    public:
        virtual bool _acquire();
        virtual void _release();

    public:
        const SoundDecoderInterfacePtr & getDecoder() const override;

    public:
        virtual bool load( const SoundDecoderInterfacePtr & _soundDecoder ) = 0;

        virtual bool playSource( ALuint _source, bool _looped, float _pos ) = 0;
        virtual bool resumeSource( ALuint _source ) = 0;
        virtual	void pauseSource( ALuint _source ) = 0;
        virtual void stopSource( ALuint _source ) = 0;

    public:
        virtual bool setTimePos( ALuint _source, float _pos ) const = 0;
        virtual bool getTimePos( ALuint _source, float & _pos ) const = 0;

    public:
        bool isStereo() const;
        float getTimeTotal() const;

    protected:
        OpenALSoundSystem * m_soundSystem;

        uint32_t m_refacquire;

        SoundDecoderInterfacePtr m_soundDecoder;

        ALenum m_format;

        int32_t m_frequency;
        int32_t m_channels;
        float m_length;
        bool m_isStereo;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenALSoundBufferBase, SoundBufferInterface> OpenALSoundBufferBasePtr;
    //////////////////////////////////////////////////////////////////////////
}

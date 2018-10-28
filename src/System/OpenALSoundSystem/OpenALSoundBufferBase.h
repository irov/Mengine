#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Kernel/Factorable.h"

#include "AL/al.h"

namespace Mengine
{
    class OALSoundSystem;

    class OALSoundBufferBase
        : public SoundBufferInterface
        , public Factorable
    {
    public:
        OALSoundBufferBase();
        ~OALSoundBufferBase() override;

    public:
        void initialize( OALSoundSystem * _soundSystem );

    public:
        const SoundDecoderInterfacePtr & getDecoder() const override;

    public:
        virtual bool load( const SoundDecoderInterfacePtr & _soundDecoder ) = 0;

        virtual bool play( ALuint _source, bool _looped, float _pos ) = 0;
        virtual bool resume( ALuint _source ) = 0;
        virtual	void pause( ALuint _source ) = 0;
        virtual void stop( ALuint _source ) = 0;

    public:
        virtual bool setTimePos( ALuint _source, float _pos ) const = 0;
        virtual bool getTimePos( ALuint _source, float & _pos ) const = 0;

    public:
        bool isStereo() const;
        float getTimeTotal() const;

    protected:
        OALSoundSystem * m_soundSystem;

        SoundDecoderInterfacePtr m_soundDecoder;

        ALenum m_format;

        int m_frequency;
        int m_channels;
        float m_length;
        bool m_isStereo;
    };

    typedef IntrusivePtr<OALSoundBufferBase> OALSoundBufferBasePtr;
}

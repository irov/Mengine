#pragma once

#include "OpenALSoundBufferBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenALSoundBufferMemory
        : public OpenALSoundBufferBase
    {
        DECLARE_FACTORABLE( OpenALSoundBufferMemory );

    public:
        OpenALSoundBufferMemory();
        ~OpenALSoundBufferMemory() override;

    public:
        bool _acquireSoundBuffer() override;
        void _releaseSoundBuffer() override;

    public:
        bool updateSoundBuffer() override;

    public:
        bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

        bool playSource( ALuint _source, bool _looped, float _pos ) override;
        bool resumeSource( ALuint _source ) override;
        void pauseSource( ALuint _source ) override;
        void stopSource( ALuint _source ) override;

    public:
        bool setTimePos( ALuint _source, float _pos ) const override;
        bool getTimePos( ALuint _source, float * const _pos ) const override;

    protected:
        ALuint m_alBufferId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenALSoundBufferMemory, SoundBufferInterface> OpenALSoundBufferMemoryPtr;
    //////////////////////////////////////////////////////////////////////////
}

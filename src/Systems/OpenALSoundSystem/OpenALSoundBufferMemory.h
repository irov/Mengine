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

        bool playSource( ALuint _sourceId, bool _looped, float _pos ) override;
        bool resumeSource( ALuint _sourceId ) override;
        void pauseSource( ALuint _sourceId ) override;
        void stopSource( ALuint _sourceId ) override;

    public:
        bool setTimePos( ALuint _sourceId, float _pos ) const override;
        bool getTimePos( ALuint _sourceId, float * const _pos ) const override;

    protected:
        ALuint m_alBufferId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenALSoundBufferMemory, SoundBufferInterface> OpenALSoundBufferMemoryPtr;
    //////////////////////////////////////////////////////////////////////////
}

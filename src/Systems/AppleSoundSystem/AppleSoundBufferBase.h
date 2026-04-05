#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/SoundCodecInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ReferenceCounter.h"

#include <AudioToolbox/AudioToolbox.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleSoundBufferBase
        : public SoundBufferInterface
    {
    public:
        AppleSoundBufferBase();
        ~AppleSoundBufferBase() override;

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

        virtual bool playSource( bool _looped, float _position ) = 0;
        virtual void stopSource() = 0;
        virtual void pauseSource() = 0;
        virtual void resumeSource() = 0;

    public:
        virtual bool setTimePosition( float _position ) = 0;
        virtual bool getTimePosition( float * const _position ) const = 0;

    public:
        virtual void setVolume( float _gain ) = 0;

    public:
        virtual uint32_t renderMixerFrames( AudioBufferList * _ioData, uint32_t _frameOffset, uint32_t _frames, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition ) = 0;

    public:
        virtual uint32_t getFrameCount() const;

    public:
        bool isStereo() const;
        uint32_t getFrequency() const;
        uint32_t getChannels() const;
        uint32_t getFrameSize() const;
        float getTimeDuration() const;

    protected:
        ReferenceCounter m_refacquire;

        SoundDecoderInterfacePtr m_soundDecoder;

        uint32_t m_frequency;
        uint32_t m_channels;
        float m_duration;
        bool m_isStereo;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleSoundBufferBase, SoundBufferInterface> AppleSoundBufferBasePtr;
    //////////////////////////////////////////////////////////////////////////
}

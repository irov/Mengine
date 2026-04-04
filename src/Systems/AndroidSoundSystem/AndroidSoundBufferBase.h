#pragma once

#include "Interface/SoundSystemInterface.h"
#include "Interface/SoundCodecInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ReferenceCounter.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidSoundBufferBase
        : public SoundBufferInterface
    {
    public:
        AndroidSoundBufferBase();
        ~AndroidSoundBufferBase() override;

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
        bool isStereo() const;
        float getTimeDuration() const;

    protected:
        bool createPlayer_();
        void destroyPlayer_();

    protected:
        static SLmillibel gainToMillibels_( float _gain );

    protected:
        ReferenceCounter m_refacquire;

        SoundDecoderInterfacePtr m_soundDecoder;

        uint32_t m_frequency;
        uint32_t m_channels;
        float m_duration;
        bool m_isStereo;

        SLObjectItf m_playerObject;
        SLPlayItf m_playItf;
        SLVolumeItf m_volumeItf;
        SLAndroidSimpleBufferQueueItf m_bufferQueueItf;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidSoundBufferBase, SoundBufferInterface> AndroidSoundBufferBasePtr;
    //////////////////////////////////////////////////////////////////////////
}

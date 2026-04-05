#pragma once

#include "AppleSoundBufferBase.h"

#include "Interface/MemoryInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleSoundBufferMemory
        : public AppleSoundBufferBase
    {
        DECLARE_FACTORABLE( AppleSoundBufferMemory );

    public:
        AppleSoundBufferMemory();
        ~AppleSoundBufferMemory() override;

    public:
        bool _acquireSoundBuffer() override;
        void _releaseSoundBuffer() override;

    public:
        bool updateSoundBuffer() override;

    public:
        bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

    public:
        bool playSource( bool _looped, float _position ) override;
        void stopSource() override;
        void pauseSource() override;
        void resumeSource() override;

    public:
        bool setTimePosition( float _position ) override;
        bool getTimePosition( float * const _position ) const override;

    public:
        void setVolume( float _gain ) override;

    public:
        uint32_t renderMixerFrames( AudioBufferList * _ioData, uint32_t _frameOffset, uint32_t _frames, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition ) override;

    public:
        const MemoryInterfacePtr & getPCMMemory() const;
        uint32_t getFrameCount() const override;

    protected:
        MemoryInterfacePtr m_pcmMemory;
        size_t m_pcmDataSize;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleSoundBufferMemory, SoundBufferInterface> AppleSoundBufferMemoryPtr;
    //////////////////////////////////////////////////////////////////////////
}

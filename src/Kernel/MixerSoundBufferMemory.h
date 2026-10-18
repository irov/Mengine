#pragma once

#include "Interface/MemoryInterface.h"

#include "Kernel/MixerSoundBufferBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MixerSoundBufferMemory
        : public MixerSoundBufferBase
    {
        DECLARE_FACTORABLE( MixerSoundBufferMemory );

    public:
        MixerSoundBufferMemory();
        ~MixerSoundBufferMemory() override;

    public:
        bool setTimePosition( float _position ) override;
        bool getTimePosition( float * const _position ) const override;

    public:
        void setVolume( float _gain ) override;

    public:
        const MemoryInterfacePtr & getPCMMemory() const;
        uint32_t getFrameCount() const override;

    public:
        bool _acquireSoundBuffer() override;
        void _releaseSoundBuffer() override;

    public:
        bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

    public:
        bool updateSoundBuffer() override;

    public:
        bool playSource( bool _looped, float _position ) override;
        void stopSource() override;
        void pauseSource() override;
        void resumeSource() override;

    public:
        uint32_t renderMixerFrames( float * _mixBuffer, uint32_t _outputChannels, uint32_t _frameOffset, uint32_t _frames, float _gain, uint32_t _framePosition, bool _loop, uint32_t * const _outFramePosition ) override;

    protected:
        MemoryInterfacePtr m_pcmMemory;
        size_t m_pcmDataSize;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MixerSoundBufferMemory, SoundBufferInterface> MixerSoundBufferMemoryPtr;
    //////////////////////////////////////////////////////////////////////////
}

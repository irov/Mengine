#pragma once

#include "AndroidSoundBufferBase.h"

#include "Interface/MemoryInterface.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidSoundBufferMemory
        : public AndroidSoundBufferBase
    {
        DECLARE_FACTORABLE( AndroidSoundBufferMemory );

    public:
        AndroidSoundBufferMemory();
        ~AndroidSoundBufferMemory() override;

    public:
        bool _acquireSoundBuffer() override;
        void _releaseSoundBuffer() override;

    public:
        bool updateSoundBuffer() override;

    public:
        bool load( const SoundDecoderInterfacePtr & _soundDecoder ) override;

        bool playSource( bool _looped, float _position ) override;
        void stopSource() override;
        void pauseSource() override;
        void resumeSource() override;

    public:
        bool setTimePosition( float _position ) override;
        bool getTimePosition( float * const _position ) const override;

    public:
        void setVolume( float _gain ) override;

    protected:
        static void bufferQueueCallback_( SLAndroidSimpleBufferQueueItf _bq, void * _context );
        void onBufferConsumed_();

    protected:
        MemoryInterfacePtr m_pcmMemory;
        size_t m_pcmDataSize;

        bool m_looped;
        Timestamp m_playTimestamp;
        Timestamp m_pauseTimestamp;
        float m_positionOffset;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidSoundBufferMemory, SoundBufferInterface> AndroidSoundBufferMemoryPtr;
    //////////////////////////////////////////////////////////////////////////
}

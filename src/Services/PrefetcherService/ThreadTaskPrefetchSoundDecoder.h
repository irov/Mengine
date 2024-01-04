#pragma once

#include "ThreadTaskPrefetch.h"

#include "Interface/SoundCodecInterface.h"

namespace Mengine
{
    class ThreadTaskPrefetchSoundDecoder
        : public ThreadTaskPrefetch
    {
        DECLARE_FACTORABLE( ThreadTaskPrefetchSoundDecoder );

    public:
        ThreadTaskPrefetchSoundDecoder();
        ~ThreadTaskPrefetchSoundDecoder() override;

    public:
        const SoundDecoderInterfacePtr & getDecoder() const;

    protected:
        void _finalize() override;

    protected:
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;

    protected:
        SoundDecoderInterfacePtr m_soundDecoder;
        MemoryInputInterfacePtr m_memoryInput;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPrefetchSoundDecoder, ThreadTaskInterface> ThreadTaskPrefetchSoundDecoderPtr;
    //////////////////////////////////////////////////////////////////////////
}
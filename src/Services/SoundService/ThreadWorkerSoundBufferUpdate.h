#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Kernel/ThreadJob.h"

#include "Config/Atomic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadWorkerSoundBufferUpdate
        : public ThreadWorkerInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( ThreadWorkerSoundBufferUpdate );

    public:
        ThreadWorkerSoundBufferUpdate();
        ~ThreadWorkerSoundBufferUpdate() override;

    public:
        void initialize( const SoundBufferInterfacePtr & _soundSource );

    public:
        bool isDone() const;

    public:
        void onThreadWorkerUpdate( UniqueId _id ) override;
        bool onThreadWorkerWork( UniqueId _id ) override;
        void onThreadWorkerDone( UniqueId _id ) override;

    protected:
        SoundBufferInterfacePtr m_soundBuffer;

        AtomicBool m_done;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadWorkerSoundBufferUpdate, ThreadWorkerInterface> ThreadWorkerSoundBufferUpdatePtr;
    //////////////////////////////////////////////////////////////////////////
}

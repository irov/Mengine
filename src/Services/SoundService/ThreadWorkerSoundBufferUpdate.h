#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Kernel/ThreadJob.h"

#include "Config/Atomic.h"

namespace Mengine
{
    class ThreadWorkerSoundBufferUpdate
        : public ThreadWorkerInterface
        , public Factorable
    {
    public:
        ThreadWorkerSoundBufferUpdate();
        ~ThreadWorkerSoundBufferUpdate() override;

    public:
        void initialize( const SoundBufferInterfacePtr & _soundSource );

    public:
        bool isDone() const;

    public:
        void onUpdate( uint32_t _id ) override;
        bool onWork( uint32_t _id ) override;
        void onDone( uint32_t _id ) override;

    protected:
        SoundBufferInterfacePtr m_soundBuffer;

        AtomicBool m_done;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadWorkerSoundBufferUpdate> ThreadWorkerSoundBufferUpdatePtr;
    //////////////////////////////////////////////////////////////////////////
}

#pragma once

#include "Config/Atomic.h"

#include "Interface/ThreadTaskInterface.h"

#include "Kernel/Visitable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadTask
        : public ThreadTaskInterface
        , public Visitable
    {
        DECLARE_VISITABLE_BASE();

    public:
        ThreadTask();
        ~ThreadTask() override;

    public:
        bool isRun() const override;
        bool isComplete() const override;
        bool isSuccessful() const override;
        bool isCancel() const override;

    public:
        void preparation() override;
        void main() override;

    public:
        bool run( const ThreadMutexInterfacePtr & _mutex ) override;
        bool cancel() override;
        bool update() override;
        void finish() override;
        void finally() override;

    public:
        void join() override;

    protected:
        virtual void _onThreadTaskPreparation();
        virtual bool _onThreadTaskProcess();

    protected:
        virtual bool _onThreadTaskRun();
        virtual void _onThreadTaskCancel();
        virtual void _onThreadTaskFinally();
        virtual void _onThreadTaskUpdate();
        virtual void _onThreadTaskJoin();

    protected:
        virtual void _onThreadTaskFinish();
        virtual void _onThreadTaskComplete( bool _successful );

    protected:
        const ThreadMutexInterfacePtr & getMutex() const;

    private:
        ThreadMutexInterfacePtr m_mutex;

        AtomicBool m_run;
        AtomicBool m_complete;
        AtomicBool m_finish;
        AtomicBool m_successful;
        AtomicBool m_cancel;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTask, ThreadTaskInterface> ThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}

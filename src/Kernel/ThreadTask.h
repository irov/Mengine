#pragma once

#include "Config/Atomic.h"

#include "Interface/ThreadTaskInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Visitable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadTask
        : public ThreadTaskInterface
        , public Factorable
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
        bool run() override;
        bool cancel() override;
        bool update() override;

    protected:
        virtual void _onPreparation();
        virtual bool _onMain();

    protected:
        virtual bool _onRun();
        virtual void _onCancel();
        virtual void _onUpdate();

    protected:
        virtual void _onComplete( bool _successful );

    private:
        AtomicBool m_run;
        AtomicBool m_complete;
        AtomicBool m_finish;
        AtomicBool m_successful;
        AtomicBool m_cancel;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTask> ThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}

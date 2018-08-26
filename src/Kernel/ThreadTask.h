#pragma once

#include "Interface/ThreadInterface.h"

#include "Kernel/ServantBase.h"
#include "Kernel/Visitable.h"

#include <atomic>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadTask
        : public ServantBase<ThreadTaskInterface>
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
        std::atomic_bool m_run;
        std::atomic_bool m_complete;
        std::atomic_bool m_finish;
        std::atomic_bool m_successful;
        std::atomic_bool m_cancel;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTask> ThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}

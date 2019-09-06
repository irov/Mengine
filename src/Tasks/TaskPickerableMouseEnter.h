#pragma once

#include "Kernel/Pickerable.h"

#include "Interface/InputHandlerInterface.h"

#include "GOAP/Task.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseEnterEvent & )> LambdaPickerMouseEnterEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskPickerableMouseEnter
        : public GOAP::Task
    {
    public:
        TaskPickerableMouseEnter( const PickerablePtr & _pickerable, const LambdaPickerMouseEnterEvent & _filter );
        ~TaskPickerableMouseEnter() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        PickerablePtr m_pickerable;

        LambdaPickerMouseEnterEvent m_filter;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskPickerableMouseEnter> TaskPickerableMouseEnterPtr;
    //////////////////////////////////////////////////////////////////////////
}
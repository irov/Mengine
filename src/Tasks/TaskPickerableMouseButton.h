#pragma once

#include "Kernel/Pickerable.h"

#include "GOAP/Task.h"

namespace Mengine
{   
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseButtonEvent & )> LambdaPickerMouseButtonEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskPickerableMouseButton
        : public GOAP::Task
    {
    public:
        TaskPickerableMouseButton( const PickerablePtr & _pickerable, EMouseCode _code, bool _isDown, const LambdaPickerMouseButtonEvent & _filter );
        ~TaskPickerableMouseButton() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        PickerablePtr m_pickerable;
        EMouseCode m_code;
        bool m_isDown;

        LambdaPickerMouseButtonEvent m_filter;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TaskPickerableMouseButton> TaskPickerableMouseButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}
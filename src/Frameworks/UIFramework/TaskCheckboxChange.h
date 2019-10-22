#pragma once

#include "Checkbox.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskCheckboxChange
        : public GOAP::Task
    {
    public:
        TaskCheckboxChange( const CheckboxPtr & _checkbox, bool _value );
        ~TaskCheckboxChange() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        CheckboxPtr m_checkbox;
        bool m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskCheckboxChange> TaskCheckboxChangePtr;
    //////////////////////////////////////////////////////////////////////////
}
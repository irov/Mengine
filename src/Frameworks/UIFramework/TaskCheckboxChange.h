#pragma once

#include "Checkbox.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskCheckboxChange
        : public GOAP::TaskInterface
    {
    public:
        TaskCheckboxChange( GOAP::Allocator * _allocator, const CheckboxPtr & _checkbox, bool _value );
        ~TaskCheckboxChange() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        CheckboxPtr m_checkbox;
        bool m_value;

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskCheckboxChange> TaskCheckboxChangePtr;
    //////////////////////////////////////////////////////////////////////////
}
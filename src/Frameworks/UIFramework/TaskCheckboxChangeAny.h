#pragma once

#include "Checkbox.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskCheckboxChangeAny
        : public GOAP::TaskInterface
    {
    public:
        TaskCheckboxChangeAny( GOAP::Allocator * _allocator, const CheckboxPtr & _checkbox );
        ~TaskCheckboxChangeAny() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        CheckboxPtr m_checkbox;

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskCheckboxChangeAny> TaskCheckboxChangeAnyPtr;
    //////////////////////////////////////////////////////////////////////////
}
#pragma once

#include "Kernel/Pickerable.h"

#include "Interface/InputHandlerInterface.h"
#include "Interface/EventationInterface.h"

#include "GOAP/Task.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseButtonEvent & )> LambdaPickerMouseButtonEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskPickerableMouseButton
        : public GOAP::Task
    {
    public:
        TaskPickerableMouseButton( const PickerablePtr & _pickerable, EMouseCode _code, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter );
        ~TaskPickerableMouseButton() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        PickerablePtr m_pickerable;
        EMouseCode m_code;
        bool m_isDown;
        bool m_isPressed;

        LambdaPickerMouseButtonEvent m_filter;

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskPickerableMouseButton> TaskPickerableMouseButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}
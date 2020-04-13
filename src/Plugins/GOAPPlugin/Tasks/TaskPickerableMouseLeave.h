#pragma once

#include "Kernel/Pickerable.h"

#include "Interface/InputHandlerInterface.h"
#include "Interface/EventationInterface.h"

#include "GOAP/TaskInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseLeaveEvent & )> LambdaPickerMouseLeaveEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskPickerableMouseLeave
        : public GOAP::TaskInterface
    {
    public:
        TaskPickerableMouseLeave( const PickerablePtr & _pickerable, const LambdaPickerMouseLeaveEvent & _filter );
        ~TaskPickerableMouseLeave() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        PickerablePtr m_pickerable;

        LambdaPickerMouseLeaveEvent m_filter;

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskPickerableMouseLeave> TaskPickerableMouseLeavePtr;
    //////////////////////////////////////////////////////////////////////////
}
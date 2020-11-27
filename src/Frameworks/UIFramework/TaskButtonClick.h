#pragma once

#include "Button.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskButtonClick
        : public GOAP::TaskInterface
    {
    public:
        TaskButtonClick( GOAP::Allocator * _allocator, const ButtonPtr & _button );
        ~TaskButtonClick() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        ButtonPtr m_button;

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskButtonClick> TaskButtonClickPtr;
    //////////////////////////////////////////////////////////////////////////
}
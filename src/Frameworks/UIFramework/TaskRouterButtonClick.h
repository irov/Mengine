#pragma once

#include "RouterButtonEventReceiver.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskRouterButtonClick
        : public GOAP::TaskInterface
    {
    public:
        TaskRouterButtonClick( const ConstString & _state, const RouterButtonEventReceiverPtr & _router );
        ~TaskRouterButtonClick() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        ConstString m_state;

        RouterButtonEventReceiverPtr m_router;
    };

    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskRouterButtonClick> TaskRouterButtonClickPtr;
    //////////////////////////////////////////////////////////////////////////
}
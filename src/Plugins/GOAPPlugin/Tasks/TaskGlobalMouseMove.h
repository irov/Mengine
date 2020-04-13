#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseMoveEvent & )> LambdaInputMouseMoveEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseMove
        : public GOAP::TaskInterface
    {
    public:
        explicit TaskGlobalMouseMove( const LambdaInputMouseMoveEvent & _filter );
        ~TaskGlobalMouseMove() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        LambdaInputMouseMoveEvent m_filter;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalMouseMove> TaskGlobalMouseMovePtr;
    //////////////////////////////////////////////////////////////////////////
}
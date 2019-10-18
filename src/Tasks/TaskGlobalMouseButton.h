#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Kernel/MouseCode.h"

#include "Config/Lambda.h"

#include "GOAP/Task.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseButtonEvent & )> LambdaInputMouseButtonEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseButton
        : public GOAP::Task
    {
    public:
        TaskGlobalMouseButton( EMouseCode _code, bool _isDown, const LambdaInputMouseButtonEvent & _filter );
        ~TaskGlobalMouseButton() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        EMouseCode m_code;
        bool m_isDown;

        LambdaInputMouseButtonEvent m_filter;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalMouseButton> TaskGlobalMouseButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}
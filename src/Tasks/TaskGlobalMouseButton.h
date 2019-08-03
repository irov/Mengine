#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Kernel/MouseCode.h"

#include "Config/Lambda.h"

#include "GOAP/Task.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseButtonEvent & )> LambdaMouseButtonEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseButton
        : public GOAP::Task
    {
    public:
        TaskGlobalMouseButton( EMouseCode _code, bool _isDown, const LambdaMouseButtonEvent & _filter );
        ~TaskGlobalMouseButton() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;
        
    protected:
        EMouseCode m_code;
        bool m_isDown;

        LambdaMouseButtonEvent m_filter;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalMouseButton> TaskGlobalMouseButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}
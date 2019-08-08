#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Kernel/KeyCode.h"

#include "Config/Lambda.h"

#include "GOAP/Task.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputKeyEvent & )> LambdaInputKeyEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalKeyPress
        : public GOAP::Task
    {
    public:
        TaskGlobalKeyPress( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _filter );
        ~TaskGlobalKeyPress() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;
        
    protected:
        EKeyCode m_code;
        bool m_isDown;

        LambdaInputKeyEvent m_filter;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalKeyPress> TaskGlobalKeyPressPtr;
    //////////////////////////////////////////////////////////////////////////
}
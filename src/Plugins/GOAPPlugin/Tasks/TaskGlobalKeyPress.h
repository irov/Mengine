#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"

#include "Kernel/KeyCode.h"

#include "Config/Lambda.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputKeyEvent & )> LambdaInputKeyEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalKeyPress
        : public GOAP::TaskInterface
    {
    public:
        TaskGlobalKeyPress( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _filter );
        ~TaskGlobalKeyPress() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
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
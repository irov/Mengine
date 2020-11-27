#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"

#include "Kernel/KeyCode.h"
#include "Kernel/Document.h"

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
        TaskGlobalKeyPress( GOAP::Allocator * _allocator, EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _filter, const DocumentPtr & _doc );
        ~TaskGlobalKeyPress() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        EKeyCode m_code;
        bool m_isDown;

        LambdaInputKeyEvent m_filter;

#if MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalKeyPress> TaskGlobalKeyPressPtr;
    //////////////////////////////////////////////////////////////////////////
}
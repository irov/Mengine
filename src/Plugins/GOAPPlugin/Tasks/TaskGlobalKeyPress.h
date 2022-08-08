#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"

#include "Kernel/KeyCode.h"
#include "Kernel/Document.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

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

#ifdef MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalKeyPress> TaskGlobalKeyPressPtr;
    //////////////////////////////////////////////////////////////////////////
}
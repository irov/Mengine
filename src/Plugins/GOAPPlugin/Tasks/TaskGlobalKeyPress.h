#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/KeyCode.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputKeyEvent & )> LambdaInputKeyFilter;
    typedef Lambda<void( const InputKeyEvent & )> LambdaInputKeyComplete;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalKeyPress
        : public GOAP::TaskInterface
    {
    public:
        TaskGlobalKeyPress( GOAP::Allocator * _allocator, EKeyCode _code, bool _isDown, const LambdaInputKeyFilter & _filter, const LambdaInputKeyComplete & _cb, const DocumentInterfacePtr & _doc );
        ~TaskGlobalKeyPress() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        EKeyCode m_code;
        bool m_isDown;

        LambdaInputKeyFilter m_filter;
        LambdaInputKeyComplete m_cb;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalKeyPress> TaskGlobalKeyPressPtr;
    //////////////////////////////////////////////////////////////////////////
}
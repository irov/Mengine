#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"

#include "Kernel/MouseCode.h"
#include "Kernel/Document.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseButtonEvent & )> LambdaInputMouseButtonEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseButton
        : public GOAP::TaskInterface
    {
    public:
        TaskGlobalMouseButton( GOAP::Allocator * _allocator, EMouseCode _code, bool _isDown, const LambdaInputMouseButtonEvent & _filter, const DocumentPtr & _doc );
        ~TaskGlobalMouseButton() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        EMouseCode m_code;
        bool m_isDown;

        LambdaInputMouseButtonEvent m_filter;

#if MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalMouseButton> TaskGlobalMouseButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}
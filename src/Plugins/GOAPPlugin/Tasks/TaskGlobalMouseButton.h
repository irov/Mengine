#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"

#include "Kernel/MouseButtonCode.h"
#include "Kernel/Document.h"

#include "Config/UniqueId.h"
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
        TaskGlobalMouseButton( GOAP::Allocator * _allocator, EMouseButtonCode _button, bool _isDown, const LambdaInputMouseButtonEvent & _filter, const DocumentPtr & _doc );
        ~TaskGlobalMouseButton() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        EMouseButtonCode m_button;
        bool m_isDown;

        LambdaInputMouseButtonEvent m_filter;

#ifdef MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalMouseButton> TaskGlobalMouseButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}
#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"

#include "Kernel/Document.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseWheelEvent & )> LambdaInputMouseWheelEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseWheel
        : public GOAP::TaskInterface
    {
    public:
        TaskGlobalMouseWheel( const LambdaInputMouseWheelEvent & _filter, const DocumentPtr & _doc );
        ~TaskGlobalMouseWheel() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        LambdaInputMouseWheelEvent m_filter;

        DocumentPtr m_doc;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalMouseWheel> TaskGlobalMouseWheelPtr;
    //////////////////////////////////////////////////////////////////////////
}
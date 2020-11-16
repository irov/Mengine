#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"

#include "Kernel/Document.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseMoveEvent & )> LambdaInputMouseMoveEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseMove
        : public GOAP::TaskInterface
    {
    public:
        TaskGlobalMouseMove( const LambdaInputMouseMoveEvent & _filter, const DocumentPtr & _doc );
        ~TaskGlobalMouseMove() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        LambdaInputMouseMoveEvent m_filter;

        DocumentPtr m_doc;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalMouseMove> TaskGlobalMouseMovePtr;
    //////////////////////////////////////////////////////////////////////////
}
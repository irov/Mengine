#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"
#include "Interface/DocumentInterface.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseMoveEvent & )> LambdaInputMouseMoveEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseMove
        : public GOAP::TaskInterface
    {
    public:
        TaskGlobalMouseMove( GOAP::Allocator * _allocator, const LambdaInputMouseMoveEvent & _filter, const DocumentInterfacePtr & _doc );
        ~TaskGlobalMouseMove() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        LambdaInputMouseMoveEvent m_filter;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalMouseMove> TaskGlobalMouseMovePtr;
    //////////////////////////////////////////////////////////////////////////
}
#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/InputHandlerInterface.h"
#include "Interface/DocumentInterface.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseWheelEvent & )> LambdaInputMouseWheelEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalMouseWheel
        : public GOAP::TaskInterface
    {
    public:
        TaskGlobalMouseWheel( GOAP::Allocator * _allocator, const LambdaInputMouseWheelEvent & _filter, const DocumentInterfacePtr & _doc );
        ~TaskGlobalMouseWheel() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        LambdaInputMouseWheelEvent m_filter;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskGlobalMouseWheel> TaskGlobalMouseWheelPtr;
    //////////////////////////////////////////////////////////////////////////
}
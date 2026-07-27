#pragma once

#include "Interface/DocumentInterface.h"

#include "Config/UniqueId.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskNextFrame
        : public GOAP::TaskInterface
    {
    public:
        TaskNextFrame( GOAP::Allocator * _allocator, const DocumentInterfacePtr & _doc );
        ~TaskNextFrame() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskNextFrame> TaskNextFramePtr;
    //////////////////////////////////////////////////////////////////////////
}

#pragma once

#include "Interface/InputHandlerInterface.h"
#include "Interface/EventationInterface.h"

#include "GOAP/TaskInterface.h"

#include "Kernel/Pickerable.h"
#include "Kernel/Document.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const InputMouseLeaveEvent & )> LambdaPickerMouseLeaveEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskPickerableMouseLeave
        : public GOAP::TaskInterface
    {
    public:
        TaskPickerableMouseLeave( GOAP::Allocator * _allocator, const PickerablePtr & _pickerable, const LambdaPickerMouseLeaveEvent & _filter, const DocumentPtr & _doc );
        ~TaskPickerableMouseLeave() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        PickerablePtr m_pickerable;

        LambdaPickerMouseLeaveEvent m_filter;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentPtr m_doc;
#endif

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskPickerableMouseLeave> TaskPickerableMouseLeavePtr;
    //////////////////////////////////////////////////////////////////////////
}

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
    typedef Lambda<bool( const InputMouseEnterEvent &, bool * )> LambdaPickerMouseEnterEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskPickerableMouseEnter
        : public GOAP::TaskInterface
    {
    public:
        TaskPickerableMouseEnter( GOAP::Allocator * _allocator, const PickerablePtr & _pickerable, const LambdaPickerMouseEnterEvent & _filter, const DocumentPtr & _doc );
        ~TaskPickerableMouseEnter() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        PickerablePtr m_pickerable;

        LambdaPickerMouseEnterEvent m_filter;

#ifdef MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskPickerableMouseEnter> TaskPickerableMouseEnterPtr;
    //////////////////////////////////////////////////////////////////////////
}

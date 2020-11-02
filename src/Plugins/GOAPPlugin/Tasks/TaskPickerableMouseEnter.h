#pragma once

#include "Kernel/Pickerable.h"

#include "Interface/InputHandlerInterface.h"
#include "Interface/EventationInterface.h"

#include "GOAP/TaskInterface.h"

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
        TaskPickerableMouseEnter( const PickerablePtr & _pickerable, const LambdaPickerMouseEnterEvent & _filter, const DocumentPtr & _doc );
        ~TaskPickerableMouseEnter() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        PickerablePtr m_pickerable;

        LambdaPickerMouseEnterEvent m_filter;

        DocumentPtr m_doc;

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskPickerableMouseEnter> TaskPickerableMouseEnterPtr;
    //////////////////////////////////////////////////////////////////////////
}
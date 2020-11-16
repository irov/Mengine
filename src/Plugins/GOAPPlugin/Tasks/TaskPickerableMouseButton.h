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
    typedef Lambda<bool( const InputMouseButtonEvent &, bool * )> LambdaPickerMouseButtonEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskPickerableMouseButton
        : public GOAP::TaskInterface
    {
    public:
        TaskPickerableMouseButton( const PickerablePtr & _pickerable, EMouseCode _code, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter, const DocumentPtr & _doc );
        ~TaskPickerableMouseButton() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        PickerablePtr m_pickerable;
        EMouseCode m_code;
        bool m_isDown;
        bool m_isPressed;

        LambdaPickerMouseButtonEvent m_filter;

        DocumentPtr m_doc;

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskPickerableMouseButton> TaskPickerableMouseButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}

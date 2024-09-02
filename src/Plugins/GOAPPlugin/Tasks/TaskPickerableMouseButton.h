#pragma once

#include "Interface/InputHandlerInterface.h"
#include "Interface/EventationInterface.h"
#include "Interface/DocumentInterface.h"

#include "GOAP/TaskInterface.h"

#include "Kernel/Pickerable.h"
#include "Kernel/RenderContext.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const RenderContext *, const InputMouseButtonEvent &, bool * )> LambdaPickerMouseButtonEvent;
    //////////////////////////////////////////////////////////////////////////
    class TaskPickerableMouseButton
        : public GOAP::TaskInterface
    {
    public:
        TaskPickerableMouseButton( GOAP::Allocator * _allocator, const PickerablePtr & _pickerable, EMouseButtonCode _button, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter, const DocumentInterfacePtr & _doc );
        ~TaskPickerableMouseButton() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;
        bool _onSkipable() const override;

    protected:
        PickerablePtr m_pickerable;
        EMouseButtonCode m_button;
        bool m_isDown;
        bool m_isPressed;

        LambdaPickerMouseButtonEvent m_filter;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        EventReceiverInterfacePtr m_receiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskPickerableMouseButton> TaskPickerableMouseButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}

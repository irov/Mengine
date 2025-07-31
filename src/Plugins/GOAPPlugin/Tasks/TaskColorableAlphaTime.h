#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/DocumentInterface.h"

#include "Kernel/Affectorable.h"
#include "Kernel/Colorable.h"

namespace Mengine
{
    class TaskColorableAlphaTime
        : public GOAP::TaskInterface
    {
    public:
        TaskColorableAlphaTime( GOAP::Allocator * _allocator, const ColorablePtr & _colorable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, const DocumentInterfacePtr & _doc );
        ~TaskColorableAlphaTime() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        ColorablePtr m_colorable;
        AffectorablePtr m_affectorable;
        EasingInterfacePtr m_easing;

        float m_to;
        float m_time;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        AffectorPtr m_affector;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskColorableAlphaTime> TaskColorableAlphaTimePtr;
    //////////////////////////////////////////////////////////////////////////
}

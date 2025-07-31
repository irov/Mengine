#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/DocumentInterface.h"

#include "TransformationRotateMode.h"

#include "Kernel/Transformable.h"
#include "Kernel/Affectorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskTransformationRotateZTime
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationRotateZTime( GOAP::Allocator * _allocator, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentInterfacePtr & _doc );
        ~TaskTransformationRotateZTime() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformablePtr m_transformable;
        AffectorablePtr m_affectorable;
        EasingInterfacePtr m_easing;

        float m_to;
        float m_time;
        ETransformationRotateMode m_mode;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        AffectorPtr m_affector;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationRotateZTime> TaskTransformationRotateZTimePtr;
    //////////////////////////////////////////////////////////////////////////
}

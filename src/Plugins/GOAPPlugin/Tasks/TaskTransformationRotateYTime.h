#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/DocumentInterface.h"

#include "TransformationRotateMode.h"

#include "Kernel/Transformable.h"
#include "Kernel/Affectorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskTransformationRotateYTime
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationRotateYTime( GOAP::Allocator * _allocator, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentInterfacePtr & _doc );
        ~TaskTransformationRotateYTime() override;

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

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationRotateYTime> TaskTransformationRotateYTimePtr;
    //////////////////////////////////////////////////////////////////////////
}

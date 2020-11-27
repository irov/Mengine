#pragma once

#include "GOAP/TaskInterface.h"

#include "Kernel/Transformation.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Document.h"

namespace Mengine
{
    class TaskTransformationRotateZ
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationRotateZ( GOAP::Allocator * _allocator, const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed, const DocumentPtr & _doc );
        ~TaskTransformationRotateZ() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformationPtr m_transformation;
        AffectorablePtr m_affectorable;

        float m_to;
        float m_speed;

#if MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationRotateZ> TaskTransformationRotateZPtr;
    //////////////////////////////////////////////////////////////////////////
}

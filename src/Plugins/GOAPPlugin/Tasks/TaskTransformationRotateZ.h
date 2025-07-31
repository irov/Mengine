#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/DocumentInterface.h"

#include "Kernel/Transformable.h"
#include "Kernel/Affectorable.h"

namespace Mengine
{
    class TaskTransformationRotateZ
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationRotateZ( GOAP::Allocator * _allocator, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed, const DocumentInterfacePtr & _doc );
        ~TaskTransformationRotateZ() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformablePtr m_transformable;
        AffectorablePtr m_affectorable;

        float m_to;
        float m_speed;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        AffectorPtr m_affector;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationRotateZ> TaskTransformationRotateZPtr;
    //////////////////////////////////////////////////////////////////////////
}

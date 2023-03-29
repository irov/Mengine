#pragma once

#include "GOAP/TaskInterface.h"

#include "Kernel/Transformable.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Document.h"

namespace Mengine
{
    class TaskTransformationRotateY
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationRotateY( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, float _to, float _speed, const DocumentPtr & _doc );
        ~TaskTransformationRotateY() override;

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
        DocumentPtr m_doc;
#endif

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationRotateY> TaskTransformationRotateYPtr;
    //////////////////////////////////////////////////////////////////////////
}

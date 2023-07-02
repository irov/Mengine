#pragma once

#include "GOAP/TaskInterface.h"

#include "Interface/DocumentInterface.h"

#include "Kernel/Transformable.h"
#include "Kernel/Affectorable.h"

namespace Mengine
{
    class TaskTransformationTranslate
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationTranslate( GOAP::Allocator * _allocator, const TransformablePtr & _transformation, const AffectorablePtr & _affectorable, const mt::vec3f & _to, float _speed, const DocumentInterfacePtr & _doc );
        ~TaskTransformationTranslate() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformablePtr m_transformable;
        AffectorablePtr m_affectorable;

        mt::vec3f m_to;
        float m_speed;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationTranslate> TaskTransformationTranslatePtr;
    //////////////////////////////////////////////////////////////////////////
}
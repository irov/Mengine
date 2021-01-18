#pragma once

#include "GOAP/TaskInterface.h"

#include "Kernel/Transformable.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Document.h"

namespace Mengine
{
    class TaskTransformationTranslateTimeWithSkip
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationTranslateTimeWithSkip( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, const DocumentPtr & _doc );
        ~TaskTransformationTranslateTimeWithSkip() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformablePtr m_transformable;
        AffectorablePtr m_affectorable;
        EasingInterfacePtr m_easing;

        mt::vec3f m_to;
        float m_time;

#if MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationTranslateTimeWithSkip> TaskTransformationTranslateTimeWithSkipPtr;
    //////////////////////////////////////////////////////////////////////////
}

#pragma once

#include "GOAP/TaskInterface.h"

#include "TaskEnum.h"

#include "Kernel/Transformation.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Document.h"

namespace Mengine
{
    class TaskTransformationScaleTime
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationScaleTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time, uint32_t _flags, const DocumentPtr & _doc );
        ~TaskTransformationScaleTime() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformationPtr m_transformation;
        AffectorablePtr m_affectorable;
        EasingInterfacePtr m_easing;

        mt::vec3f m_to;
        float m_time;

        uint32_t m_flags;

        DocumentPtr m_doc;

        uint32_t m_id;        
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationScaleTime> TaskTransformationScaleTimePtr;
    //////////////////////////////////////////////////////////////////////////
}

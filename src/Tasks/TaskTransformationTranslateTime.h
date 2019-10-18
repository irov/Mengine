#pragma once

#include "GOAP/Task.h"

#include "Kernel/Transformation.h"
#include "Kernel/Affectorable.h"

namespace Mengine
{
    class TaskTransformationTranslateTime
        : public GOAP::Task
    {
    public:
        TaskTransformationTranslateTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const mt::vec3f & _to, float _time );
        ~TaskTransformationTranslateTime() override;

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

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationTranslateTime> TaskTransformationTranslateTimePtr;
    //////////////////////////////////////////////////////////////////////////
}
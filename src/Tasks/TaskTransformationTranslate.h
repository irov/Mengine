#pragma once

#include "GOAP/Task.h"

#include "Kernel/Transformation.h"
#include "Kernel/Affectorable.h"

namespace Mengine
{
    class TaskTransformationTranslate
        : public GOAP::Task
    {
    public:
        TaskTransformationTranslate( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const mt::vec3f & _to, float _speed );
        ~TaskTransformationTranslate() override;

    protected:
        bool _onRun() override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformationPtr m_transformation;
        AffectorablePtr m_affectorable;

        mt::vec3f m_to;
        float m_speed;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TaskTransformationTranslate> TaskTransformationTranslatePtr;
    //////////////////////////////////////////////////////////////////////////
}
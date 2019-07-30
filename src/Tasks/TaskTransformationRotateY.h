#pragma once

#include "GOAP/Task.h"

#include "Kernel/Transformation.h"
#include "Kernel/Affectorable.h"

namespace Mengine
{
    class TaskTransformationRotateY
        : public GOAP::Task
    {
    public:
        TaskTransformationRotateY( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, float _to, float _speed );
        ~TaskTransformationRotateY() override;

    protected:
        bool _onRun() override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformationPtr m_transformation;
        AffectorablePtr m_affectorable;

        float m_to;
        float m_speed;
        
        AFFECTOR_ID m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationRotateY> TaskTransformationRotateYPtr;
    //////////////////////////////////////////////////////////////////////////
}
#pragma once

#include "GOAP/TaskInterface.h"

#include "TransformationRotateMode.h"

#include "Kernel/Transformation.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskTransformationRotateXTime
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationRotateXTime( const TransformationPtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentPtr & _doc );
        ~TaskTransformationRotateXTime() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformationPtr m_transformation;
        AffectorablePtr m_affectorable;
        EasingInterfacePtr m_easing;

        float m_to;
        float m_time;
        ETransformationRotateMode m_mode;

        DocumentPtr m_doc;

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationRotateXTime> TaskTransformationRotateXTimePtr;
    //////////////////////////////////////////////////////////////////////////
}

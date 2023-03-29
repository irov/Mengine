#pragma once

#include "GOAP/TaskInterface.h"

#include "TransformationRotateMode.h"

#include "Kernel/Transformable.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskTransformationRotateXTime
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationRotateXTime( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, float _to, float _time, ETransformationRotateMode _mode, const DocumentPtr & _doc );
        ~TaskTransformationRotateXTime() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformablePtr m_transformable;
        AffectorablePtr m_affectorable;
        EasingInterfacePtr m_easing;

        float m_to;
        float m_time;
        ETransformationRotateMode m_mode;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentPtr m_doc;
#endif

        UniqueId m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationRotateXTime> TaskTransformationRotateXTimePtr;
    //////////////////////////////////////////////////////////////////////////
}

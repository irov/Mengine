#pragma once

#include "GOAP/TaskInterface.h"

#include "AffectorTransformationPath.h"

#include "Interface/DocumentInterface.h"

#include "Kernel/Affectorable.h"

namespace Mengine
{
    class TaskTransformationPath
        : public GOAP::TaskInterface
    {
    public:
        TaskTransformationPath( GOAP::Allocator * _allocator, const TransformablePtr & _transformable, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const Vector<mt::vec3f> & _points, float _duration, ETransformationPathMode _mode, const DocumentInterfacePtr & _doc );
        ~TaskTransformationPath() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        TransformablePtr m_transformable;
        AffectorablePtr m_affectorable;
        EasingInterfacePtr m_easing;
        Vector<mt::vec3f> m_points;
        float m_duration;
        ETransformationPathMode m_mode;

#if defined(MENGINE_DOCUMENT_ENABLE)
        DocumentInterfacePtr m_doc;
#endif

        AffectorPtr m_affector;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationPath> TaskTransformationPathPtr;
    //////////////////////////////////////////////////////////////////////////
}

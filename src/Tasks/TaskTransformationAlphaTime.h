#pragma once

#include "GOAP/Task.h"

#include "Interface/RenderInterface.h"

#include "Kernel/Transformation.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Colorable.h"

namespace Mengine
{
    class TaskTransformationAlphaTime
        : public GOAP::Task
    {
    public:
        TaskTransformationAlphaTime( const RenderInterfacePtr & _transformation, const AffectorablePtr & _affectorable, const EasingInterfacePtr & _easing, const float _to, float _time );
        ~TaskTransformationAlphaTime() override;

    protected:
        bool _onRun() override;
        void _onSkip() override;
        void _onFinally() override;

    protected:
        RenderInterfacePtr m_renderable;
        AffectorablePtr m_affectorable;
        EasingInterfacePtr m_easing;

        float m_to;
        float m_time;

        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskTransformationAlphaTime> TaskTransformationAlphaTimePtr;
    //////////////////////////////////////////////////////////////////////////
}
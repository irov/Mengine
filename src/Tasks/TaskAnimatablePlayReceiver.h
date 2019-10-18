#pragma once

#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/Factorable.h"
#include "Kernel/FactorableUnique.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    class TaskAnimatablePlayReceiver
        : public AnimationEventReceiverInterface
        , public Factorable
    {
    public:
        TaskAnimatablePlayReceiver();
        ~TaskAnimatablePlayReceiver() override;

    public:
        void setGOAPNode( GOAP::NodeInterface * _node );

    protected:
        void onAnimationPlay( uint32_t _enumerator, float _time ) override;
        void onAnimationRestart( uint32_t _enumerator, float _time ) override;
        void onAnimationPause( uint32_t _enumerator ) override;
        void onAnimationResume( uint32_t _enumerator, float _time ) override;
        void onAnimationStop( uint32_t _enumerator ) override;
        void onAnimationEnd( uint32_t _enumerator ) override;
        void onAnimationInterrupt( uint32_t _enumerator ) override;

    protected:
        GOAP::NodeInterfacePtr m_node;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TaskAnimatablePlayReceiver> TaskAnimatablePlayReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}


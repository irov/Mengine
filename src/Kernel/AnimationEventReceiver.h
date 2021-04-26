#pragma once

#include "Interface/EventationInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum AnimationEventFlag
    {
        EVENT_ANIMATION_PLAY = 0,
        EVENT_ANIMATION_RESTART,
        EVENT_ANIMATION_PAUSE,
        EVENT_ANIMATION_RESUME,
        EVENT_ANIMATION_STOP,
        EVENT_ANIMATION_END,
        EVENT_ANIMATION_INTERRUPT,

        __EVENT_ANIMATION_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class AnimationEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onAnimationPlay( uint32_t _enumerator, float _time ) = 0;
        virtual void onAnimationRestart( uint32_t _enumerator, float _time ) = 0;
        virtual void onAnimationPause( uint32_t _enumerator ) = 0;
        virtual void onAnimationResume( uint32_t _enumerator, float _time ) = 0;
        virtual void onAnimationStop( uint32_t _enumerator ) = 0;
        virtual void onAnimationEnd( uint32_t _enumerator ) = 0;
        virtual void onAnimationInterrupt( uint32_t _enumerator ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( AnimationEventReceiverInterface, EVENT_ANIMATION_PLAY );
    EVENTATION_TYPEID( AnimationEventReceiverInterface, EVENT_ANIMATION_RESTART );
    EVENTATION_TYPEID( AnimationEventReceiverInterface, EVENT_ANIMATION_PAUSE );
    EVENTATION_TYPEID( AnimationEventReceiverInterface, EVENT_ANIMATION_RESUME );
    EVENTATION_TYPEID( AnimationEventReceiverInterface, EVENT_ANIMATION_STOP );
    EVENTATION_TYPEID( AnimationEventReceiverInterface, EVENT_ANIMATION_END );
    EVENTATION_TYPEID( AnimationEventReceiverInterface, EVENT_ANIMATION_INTERRUPT );
    //////////////////////////////////////////////////////////////////////////
}
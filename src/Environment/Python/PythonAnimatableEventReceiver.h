#pragma once

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/Animatable.h"
#include "Kernel/AnimationEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T_AnimatableEventReceiver = AnimationEventReceiverInterface>
    class PythonAnimatableEventReceiver
        : public PythonEventReceiver
        , public T_AnimatableEventReceiver
        , public Factorable
    {
    public:
        void onAnimationPlay( UniqueId _enumerator, float _time ) override
        {
            m_cb.call( _enumerator, _time );
        }

        void onAnimationRestart( UniqueId _enumerator, float _time ) override
        {
            m_cb.call( _enumerator, _time );
        }

        void onAnimationPause( UniqueId _enumerator ) override
        {
            m_cb.call( _enumerator );
        }

        void onAnimationResume( UniqueId _enumerator, float _time ) override
        {
            m_cb.call( _enumerator, _time );
        }

        void onAnimationStop( UniqueId _enumerator ) override
        {
            m_cb.call( _enumerator );
        }

        void onAnimationEnd( UniqueId _enumerator ) override
        {
            m_cb.call( _enumerator );
        }

        void onAnimationInterrupt( UniqueId _enumerator ) override
        {
            m_cb.call( _enumerator );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T_AnimatableReceiver = PythonAnimatableEventReceiver<> >
        void registerAnimatableEventReceiver( pybind::kernel_interface * _kernel, const pybind::dict & _kwds, Eventable * const _eventable, const DocumentPtr & _doc )
        {
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, STRINGIZE_STRING_LOCAL( "onAnimatablePlay" ), EVENT_ANIMATION_PLAY, _doc );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, STRINGIZE_STRING_LOCAL( "onAnimatableRestart" ), EVENT_ANIMATION_RESTART, _doc );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, STRINGIZE_STRING_LOCAL( "onAnimatablePause" ), EVENT_ANIMATION_PAUSE, _doc );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, STRINGIZE_STRING_LOCAL( "onAnimatableResume" ), EVENT_ANIMATION_RESUME, _doc );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, STRINGIZE_STRING_LOCAL( "onAnimatableStop" ), EVENT_ANIMATION_STOP, _doc );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, STRINGIZE_STRING_LOCAL( "onAnimatableEnd" ), EVENT_ANIMATION_END, _doc );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, STRINGIZE_STRING_LOCAL( "onAnimatableInterrupt" ), EVENT_ANIMATION_INTERRUPT, _doc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
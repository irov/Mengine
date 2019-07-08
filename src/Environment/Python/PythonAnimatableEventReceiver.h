#pragma once

#include "PythonEventReceiver.h"

#include "Kernel/Animatable.h"
#include "Kernel/AnimationEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T_AnimatableEventReceiver = AnimationEventReceiver>
    class PythonAnimatableEventReceiver
        : public PythonEventReceiver
        , public T_AnimatableEventReceiver
        , public Factorable
    {
    public:
        void onAnimationPlay( uint32_t _enumerator, float _time ) override
        {
            m_cb.call( _enumerator, _time );
        }

        void onAnimationRestart( uint32_t _enumerator, float _time ) override
        {
            m_cb.call( _enumerator, _time );
        }

        void onAnimationPause( uint32_t _enumerator ) override
        {
            m_cb.call( _enumerator );
        }

        void onAnimationResume( uint32_t _enumerator, float _time ) override
        {
            m_cb.call( _enumerator, _time );
        }

        void onAnimationStop( uint32_t _enumerator ) override
        {
            m_cb.call( _enumerator );
        }

        void onAnimationEnd( uint32_t _enumerator ) override
        {
            m_cb.call( _enumerator );
        }

        void onAnimationInterrupt( uint32_t _enumerator ) override
        {
            m_cb.call( _enumerator );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T_AnimatableReceiver = PythonAnimatableEventReceiver<> >
        void registerAnimatableEventReceiver( pybind::kernel_interface * _kernel, const pybind::dict & _kwds, Eventable * _eventable )
        {
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, "onAnimatablePlay", EVENT_ANIMATION_PLAY );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, "onAnimatableRestart", EVENT_ANIMATION_RESTART );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, "onAnimatablePause", EVENT_ANIMATION_PAUSE );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, "onAnimatableResume", EVENT_ANIMATION_RESUME );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, "onAnimatableStop", EVENT_ANIMATION_STOP );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, "onAnimatableEnd", EVENT_ANIMATION_END );
            Helper::registerPythonEventReceiver<T_AnimatableReceiver>( _kernel, _kwds, _eventable, "onAnimatableInterrupt", EVENT_ANIMATION_INTERRUPT );
        }
    }
}
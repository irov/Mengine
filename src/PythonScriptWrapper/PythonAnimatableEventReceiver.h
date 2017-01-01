#   pragma once

#   include "PythonEventReceiver.h"

#   include "Kernel/Animatable.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    template<class T_AnimatableEventReceiver = AnimatableEventReceiver>
    class PythonAnimatableEventReceiver
        : public PythonEventReceiver
        , public T_AnimatableEventReceiver
    {
    public:
        void onAnimatablePlay( uint32_t _enumerator, float _time ) override
        {
            m_cb.call( _enumerator, _time );
        }

        void onAnimatableRestart( uint32_t _enumerator, float _time ) override
        {
            m_cb.call( _enumerator, _time );
        }

        void onAnimatablePause( uint32_t _enumerator ) override
        {
            m_cb.call( _enumerator );
        }

        void onAnimatableResume( uint32_t _enumerator, float _time ) override
        {
            m_cb.call( _enumerator, _time );
        }

        void onAnimatableStop( uint32_t _enumerator ) override
        {
            m_cb.call( _enumerator );
        }

        void onAnimatableEnd( uint32_t _enumerator ) override
        {
            m_cb.call( _enumerator );
        }

        void onAnimatableInterrupt( uint32_t _enumerator ) override
        {
            m_cb.call( _enumerator );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T_AnimatableReceiver = PythonAnimatableEventReceiver<>>
        void registerAnimatableEventReceiver( const pybind::dict & _kwds, Eventable * _eventable )
        {
            registerEventReceiver<T_AnimatableReceiver>( _kwds, _eventable, "onAnimatablePlay", EVENT_ANIMATABLE_PLAY );
            registerEventReceiver<T_AnimatableReceiver>( _kwds, _eventable, "onAnimatableRestart", EVENT_ANIMATABLE_RESTART );
            registerEventReceiver<T_AnimatableReceiver>( _kwds, _eventable, "onAnimatablePause", EVENT_ANIMATABLE_PAUSE );
            registerEventReceiver<T_AnimatableReceiver>( _kwds, _eventable, "onAnimatableResume", EVENT_ANIMATABLE_RESUME );
            registerEventReceiver<T_AnimatableReceiver>( _kwds, _eventable, "onAnimatableStop", EVENT_ANIMATABLE_STOP );
            registerEventReceiver<T_AnimatableReceiver>( _kwds, _eventable, "onAnimatableEnd", EVENT_ANIMATABLE_END );
            registerEventReceiver<T_AnimatableReceiver>( _kwds, _eventable, "onAnimatableInterrupt", EVENT_ANIMATABLE_INTERRUPT );
        }
    }
}
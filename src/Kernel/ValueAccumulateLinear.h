#pragma once

#include "Kernel/ValueAccumulator.h"

#include "math/utils.h"

namespace Mengine
{
    template<typename T>
    class ValueAccumulateLinear
        : public ValueAccumulator<T>
    {
    public:
        void start( const T & _position, const T & _accumulate, float _speed )
        {
            MENGINE_ASSERTION( _speed >= 0.f, "speed must be more or equal 0.f" );

            ValueAccumulator<T>::m_started = false;

            ValueAccumulator<T>::m_value = _position;
            ValueAccumulator<T>::m_delta = _accumulate * _speed;
            ValueAccumulator<T>::m_time = 0.f;

            ValueAccumulator<T>::m_started = true;
        }

        bool update( const EasingInterfacePtr & _easing, const UpdateContext * _context, T * const _out, float * const _used ) override
        {
            MENGINE_UNUSED( _easing );

            *_used = _context->time;

            if( ValueAccumulator<T>::m_started == false )
            {
                *_out = ValueAccumulator<T>::m_value;

                return true;
            }

            ValueAccumulator<T>::m_time += _context->time;

            ValueAccumulator<T>::m_value += ValueAccumulator<T>::m_delta * _context->time;

            *_out = ValueAccumulator<T>::m_value;

            return false;
        }
    };
}

#pragma once

#include "Kernel/ValueInterpolator.h"

namespace Mengine
{
    template<typename T>
    class ValueInterpolatorLinear
        : public ValueInterpolator<T>
    {
    public:
        void _update( float _dt, T * const _out ) override
        {
            *_out = ValueInterpolator<T>::m_value1 + (ValueInterpolator<T>::m_value2 - ValueInterpolator<T>::m_value1) * _dt;
        }
    };
}

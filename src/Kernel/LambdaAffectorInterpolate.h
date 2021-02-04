#pragma once

#include "Kernel/LambdaAffector.h"

namespace Mengine
{
    template<class T, class Interpolator>
    class LambdaAffectorInterpolate
        : public LambdaAffector<T>
    {
    protected:
        Interpolator & getInterpolator()
        {
            return m_interpolator;
        }

    protected:
        bool _affect( const UpdateContext * _context, float * const _used ) override
        {
            const EasingInterfacePtr & easing = this->getEasing();

            T value;
            bool finish = m_interpolator.update( easing, _context, &value, _used );

            this->callSettuper( value );

            return finish;
        }

        void _stop() override
        {
            m_interpolator.stop();
        }

    protected:
        Interpolator m_interpolator;
    };
}

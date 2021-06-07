#pragma once

#include "Kernel/LambdaAffectorInterpolate.h"
#include "Kernel/ValueAccumulateLinear.h"

namespace Mengine
{
    template<class T>
    class LambdaAffectorAccumulateLinear
        : public LambdaAffectorInterpolate<T, ValueAccumulateLinear<T>>
    {
        DECLARE_FACTORABLE( LambdaAffectorAccumulateLinear );

    public:
        typedef typename LambdaAffector<T>::LambdaSettuper LambdaSettuper;

    public:
        void initialize( const LambdaSettuper & _lambda, const T & _start, const T & _dir, float _speed )
        {
            this->setSettuper( _lambda );

            ValueAccumulateLinear<T> & interpolator = this->getInterpolator();

            interpolator.start( _start, _dir, _speed );
        }
    };
}

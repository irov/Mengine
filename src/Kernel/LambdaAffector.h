#pragma once

#include "Config/Lambda.h"

#include "Kernel/BaseAffector.h"

namespace Mengine
{
    template<class T>
    class LambdaAffector
        : public BaseAffector
    {
    public:
        typedef Lambda<void( const T & )> LambdaSettuper;

    public:
        LambdaAffector()
        {
        }

        ~LambdaAffector() override
        {
        }

    public:
        void setSettuper( const LambdaSettuper & _lambda )
        {
            m_settuper = _lambda;
        }

        const LambdaSettuper & getSettuper() const
        {
            return m_settuper;
        }

        void callSettuper( const T & _value )
        {
            m_settuper( _value );
        }

    protected:
        LambdaSettuper m_settuper;
    };
}

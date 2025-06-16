#pragma once

#include "Kernel/Vector.h"

namespace Mengine
{
    namespace Helper
    {
        template<class T>
        class VectorAuxScope
        {
        public:
            typedef Vector<T> ScopeType;

        public:
            VectorAuxScope( ScopeType & _scope )
                : m_scope( _scope )
            {
            }

            ~VectorAuxScope()
            {
                m_scope.clear();
            }

        protected:
            ScopeType & m_scope;
        };
    }
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_VECTOR_AUX_I(Vector, Index) Helper::VectorAuxScope<decltype(Vector)::value_type> MENGINE_PP_CONCATENATE(scope_, Index)( Vector )
#define MENGINE_VECTOR_AUX(Vector) MENGINE_VECTOR_AUX_I(Vector, MENGINE_CODE_COUNTER)
//////////////////////////////////////////////////////////////////////////
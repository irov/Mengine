#pragma once

#include "Kernel/Factory.h"
#include "Kernel/Typename.h"
#include "Kernel/Pool.h"

namespace Mengine
{
    template<class Type, uint32_t Count, class F = Factory>
    class FactoryPool
        : public F
    {
    public:
        FactoryPool()
            : F( Typename<Type>::value )
        {
        }

        ~FactoryPool() override
        {
        }

    protected:
        Factorable * _createObject() override
        {
            Type * ptr = m_pool.createT();

            return ptr;
        }

        void _destroyObject( Factorable * _node ) override
        {
            Type * ptr = static_cast<Type *>(_node);

            m_pool.destroyT( ptr );
        }

    protected:
        typedef Pool<Type, Count> PoolType;
        PoolType m_pool;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, uint32_t Count, class F = Factory>
        IntrusivePtr<F> makeFactoryPool()
        {
            Factory * factory = new FactoryPool<Type, Count, F>();

            return IntrusivePtr<F>( factory );
        }
    }
}

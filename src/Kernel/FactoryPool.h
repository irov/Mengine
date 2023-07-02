#pragma once

#include "Kernel/Factory.h"
#include "Kernel/Typename.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/Pool.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Type, uint32_t Count, class F = Factory>
    class FactoryPool
        : public F
    {
    public:
        FactoryPool()
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
        IntrusivePtr<F> makeFactoryPool( const DocumentInterfacePtr & _doc )
        {
            IntrusivePtr<F> factory = Helper::makeFactorableUnique<FactoryPool<Type, Count, F>>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            const ConstString & type = Type::getFactorableType();

            factory->initialize( type );

            return IntrusivePtr<F>( factory );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

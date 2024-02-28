#pragma once

#include "Kernel/FactoryWithoutMutex.h"
#include "Kernel/Typename.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/Pool.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Type, uint32_t Count, class F = FactoryWithoutMutex>
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
        template<class Type, uint32_t Count, class F = FactoryWithoutMutex>
        FactoryInterfacePtr makeFactoryPool( const DocumentInterfacePtr & _doc )
        {
            FactoryInterfacePtr factory = Helper::makeFactorableUnique<FactoryPool<Type, Count, F>>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            const ConstString & type = Type::getFactorableType();

            factory->setType( type );

            return factory;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

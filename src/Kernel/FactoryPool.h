#pragma once

#include "Kernel/FactoryWithMutex.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/Typename.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/Pool.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Type, uint32_t Count, class F = FactoryWithMutex>
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
        //////////////////////////////////////////////////////////////////////////
        template<class Type, uint32_t Count, class F = FactoryWithMutex>
        FactoryInterfacePtr makeFactoryPoolWithoutMutex( const DocumentInterfacePtr & _doc )
        {
            FactoryInterfacePtr factory = Helper::makeFactorableUnique<FactoryPool<Type, Count, F>>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid create factory '%s' pool %d"
                , Typename<Type>::value
                , Count
            );

            const ConstString & type = Type::getFactorableType();

            factory->setType( type );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class Type, uint32_t Count, class F = FactoryWithMutex>
        FactoryInterfacePtr makeFactoryPoolWithMutex( const ThreadMutexInterfacePtr & _mutex, const DocumentInterfacePtr & _doc )
        {
            FactoryInterfacePtr factory = Helper::makeFactoryPoolWithoutMutex<Type, Count, F>( _doc );

            factory->setMutex( _mutex );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class Type, uint32_t Count, class F = FactoryWithMutex>
        FactoryInterfacePtr makeFactoryPool( const DocumentInterfacePtr & _doc )
        {
            ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( _doc );

            FactoryInterfacePtr factory = Helper::makeFactoryPoolWithMutex<Type, Count, F>( mutex, _doc );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}

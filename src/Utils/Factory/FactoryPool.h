#	pragma once

#	include "Factory/Factory.h"
#   include "Factory/FactoryHelper.h"

#	include "stdex/pool.h"

#   include <typeinfo>

namespace Menge
{
	template<class Type, size_t Count>
	class FactoryPool
		: public Factory
	{
    public:
        FactoryPool( ServiceProviderInterface * _serviceProvider )
            : Factory( _serviceProvider, typeid(Type).name() )
        {
        }

        ~FactoryPool()
        {
        }

    protected:
        Factorable * _createObject() override
        {
            Type * ptr = m_pool.createT();

			new (ptr) Type();
				 
            return ptr;
        }

		void _destroyObject( Factorable * _node ) override
		{
            Type * ptr = static_cast<Type*>(_node);

			ptr->~Type();

            m_pool.destroyT( ptr );
		}

	protected:
        typedef stdex::template_pool<Type, Count> TTemplatePool;
		TTemplatePool m_pool;
	};
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class Type, size_t Count, class C, class M>
        FactoryPtr makeFactoryPool( ServiceProviderInterface * _serviceProvider, C * _self, M _method )
        {
            FactoryPtr factory = new FactoryPool<Type, Count>( _serviceProvider );

            setupFactoryDestroyListener<Type>( factory, _self, _method );

            return factory;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

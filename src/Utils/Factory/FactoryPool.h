#pragma once

#include "Factory/Factory.h"

#include "stdex/pool.h"

#include <typeinfo>

namespace Mengine
{
	template<class Type, size_t Count, class F = Factory>
	class FactoryPool
		: public F
	{
    public:
        FactoryPool()
            : F( typeid(Type).name() )
        {
        }

        ~FactoryPool()
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
            Type * ptr = static_cast<Type*>(_node);

            m_pool.destroyT( ptr );
		}

	protected:
        typedef stdex::template_pool<Type, Count> TTemplatePool;
		TTemplatePool m_pool;
	};
    //////////////////////////////////////////////////////////////////////////
}

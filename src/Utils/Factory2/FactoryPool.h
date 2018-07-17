#pragma once

#include "Factory/Factory.h"

#include "stdex/template_pool.h"

#include <typeinfo>

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
            Type * ptr = static_cast<Type*>(_node);

            m_pool.destroyT( ptr );
		}

	protected:
        typedef stdex::template_pool<Type, Count> TTemplatePool;
		TTemplatePool m_pool;
	};
    //////////////////////////////////////////////////////////////////////////
}

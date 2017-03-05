#	pragma once

#	include "Factory/Factory.h"
#   include "Factory/FactoryHelper.h"

#	include "stdex/pool.h"

namespace Menge
{
	template<class T, size_t Count>
	class FactoryPool
		: public Factory
	{
    public:
        FactoryPool()
        {
        }

        ~FactoryPool()
        {
        }

    protected:
        Factorable * _createObject() override
        {
            T * ptr = m_pool.createT();

            return ptr;
        }

		void _destroyObject( Factorable * _node ) override
		{
            T * ptr = static_cast<T*>(_node);

            m_pool.destroyT( ptr );
		}

	protected:
        typedef stdex::template_pool<T, Count> TTemplatePool;
		TTemplatePool m_pool;
	};
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T, size_t Count, class C, class M>
        FactoryPtr makeFactoryPool( C * _self, M _method )
        {
            FactoryPtr factory = new FactoryPool<T, Count>();

            setupFactoryDestroyListener<T>( factory, _self, _method );

            return factory;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

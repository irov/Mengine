#	pragma once

#	include "Factory/Factory.h"

#	include "stdex/pool.h"

#	ifdef _DEBUG
#	include <typeinfo>
#	endif

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
#	ifdef _DEBUG
			if( m_pool.empty() == false )
			{
				printf("FactoryPool remove object but pool is not empty!!! %s\n"
					, typeid(this).name()
					);
			}
#	endif
        }

	public:
        T * createObjectT()
        {
            Factorable * obj = this->createObject();

            T * t = static_cast<T *>(obj);

            return t;
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
}

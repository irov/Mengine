#	pragma once

#	include "Factory/Factory.h"

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

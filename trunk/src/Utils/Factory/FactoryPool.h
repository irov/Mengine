#	pragma once

#	include "Factory.h"
#	include "Core/Pool.h"

namespace Menge
{
	template<class T, size_t Count>
	class FactoryPool
		: public Factory
	{
	public:
        T * createObjectT()
        {
            Factorable * obj = this->createObject();

            return static_cast<T *>(obj);
        }

    protected:
        Factorable * _createObject() override
        {
            void * ptr = m_pool.alloc();

            T * node = new (ptr) T();

            return node;
        }

		void _destroyObject( Factorable * _node ) override
		{
            static_cast<T*>(_node)->~T();

            m_pool.free( _node );
		}

	protected:
		typedef TemplatePool<sizeof(T), Count> TNodePool;
		TNodePool m_pool;
	};
}

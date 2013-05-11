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
        FactoryPool()
        {
        }

        ~FactoryPool()
        {
#   ifdef _DEBUG
            TVectorAllockBlock & allock_blocks = m_pool.allock_blocks();

            for( TVectorAllockBlock::iterator
                it = allock_blocks.begin(),
                it_end = allock_blocks.end();
            it != it_end;
            ++it )
            {
                void * impl = *it;

                T * obj = static_cast<T *>(impl);

                static_cast<T*>(obj)->~T();
            }
#   endif
        }

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

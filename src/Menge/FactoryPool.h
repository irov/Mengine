#	pragma once

#	include "Factory.h"
#	include "Pool.h"

namespace Menge
{
	template<class T>
	class FactoryPool
		: public Factory
	{
	public:
		Factorable * _createObject() override
		{
			T * t = this->getNode();
			return t;
		}

	public:
		void destroyObject( Factorable * _node ) override
		{
			m_pool.release( static_cast<T*>(_node) );
		}

	protected:
		T * getNode()
		{
			T * t = m_pool.get();

			return m_pool.get();
		}

	protected:
		typedef Pool<T> TNodePool;
		TNodePool m_pool;
	};
}
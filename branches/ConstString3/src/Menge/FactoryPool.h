#	pragma once

#	include "Factory.h"
#	include "Core/Pool.h"

namespace Menge
{
	template<class T>
	class FactoryPool
		: public Factory
	{
	public:
		Factorable * _createObject( const ConstString & _type ) override
		{
			T * t = this->getNode();
			return t;
		}

	public:
		void _destroyObject( Factorable * _node ) override
		{
			//delete _node;
			m_pool.release( static_cast<T*>(_node) );
		}

	protected:
		T * getNode()
		{
			//return new T;
			T * t = m_pool.get();
			return t;
		}

	protected:
		typedef Pool<T, PoolPlacementPolicyErase> TNodePool;
		TNodePool m_pool;
	};

	namespace Helper
	{
		template<class T>
		Factory * createFactoryPool()
		{
			return new FactoryPool<T>();
		}
	}
}

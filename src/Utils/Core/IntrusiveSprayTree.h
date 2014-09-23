#	pragma once

#	include "Factory/FactoryStore.h"

#	include <stdex/intrusive_splay_tree.h>

namespace Menge
{
	template<class T, uint32_t Count>
	class IntrusiveSprayTree
		: public stdex::intrusive_splay_tree<T, false, false>
	{
	public:
		IntrusiveSprayTree()
		{
		}

		~IntrusiveSprayTree()
		{
			this->clear();
		}

	public:
		T * create()
		{
			T * node = m_factoryNode.createObjectT();

			return node;
		}

		void clear()
		{
			FIntrusiveSprayTreeForeachDestroy fistfd;
			this->foreach( fistfd );

			stdex::intrusive_splay_tree<T, false, false>::clear();
		}
		
	public:
		template<class C, class M>
		void setMethodListener( C * _self, M _method )
		{
			FactoryListenerInterfacePtr listener = new MethodFactoryListenerInterface<C, M, T>(_self, _method);

			m_factoryNode.setListener( listener );
		}

	protected:
		class FIntrusiveSprayTreeForeachDestroy
		{
		public:
			void operator() ( T * _node )
			{
				_node->destroy();
			}
		};
				
	protected:
		typedef FactoryPoolStore<T, Count> TFactoryNode;
		TFactoryNode m_factoryNode;
	};

	template<class T, uint32_t Count>
	class IntrusiveSprayTreePtr
		: public stdex::intrusive_splay_tree<T, false, false>
	{
	public:
		IntrusiveSprayTreePtr()
		{
			m_factoryNode.setMethodListener( this, &IntrusiveSprayTreePtr::onNodeDestroy_ );
		}

		~IntrusiveSprayTreePtr()
		{
		}

	public:
		T * create()
		{
			T * node = m_factoryNode.createObjectT();

			return node;
		}

	protected:
		void onNodeDestroy_( T * _t )
		{
			this->erase( _t );
		}

	protected:
		typedef FactoryPoolStore<T, Count> TFactoryNode;
		TFactoryNode m_factoryNode;
	};
}
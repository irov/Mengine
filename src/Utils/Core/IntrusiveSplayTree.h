#	pragma once

#	include "Factory/FactoryStore.h"

#	include <stdex/intrusive_splay_tree.h>

namespace Menge
{
	template<class T, uint32_t Count>
	class IntrusiveSplayTree
		: public stdex::intrusive_splay_tree<T, false, false>
	{
	public:
		IntrusiveSplayTree()
		{
		}

		~IntrusiveSplayTree()
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
			FIntrusiveSplayTreeForeachDestroy fistfd;
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
		class FIntrusiveSplayTreeForeachDestroy
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
	class IntrusiveSplayTreePtr
		: public stdex::intrusive_splay_tree<T, false, false>
	{
	public:
		IntrusiveSplayTreePtr()
		{
			m_factoryNode.setMethodListener( this, &IntrusiveSplayTreePtr::onNodeDestroy_ );
		}

		~IntrusiveSplayTreePtr()
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
#	pragma once

#	include "Factory/FactoryStore.h"

#	include <stdex/intrusive_tree.h>

namespace Menge
{
	template<class T, uint32_t Count>
	class IntrusiveTree
		: public stdex::intrusive_tree<T>
	{
	public:
		IntrusiveTree()
		{
		}

		~IntrusiveTree()
		{
			this->clear();
		}

	public:
		T * create()
		{
			T * node = m_factoryNode.createObject();

			return node;
		}

		void clear()
		{
			FIntrusiveTreeForeachDestroy fistfd;
			this->foreach( fistfd );

			stdex::intrusive_tree<T>::clear();
		}
		
	public:
		template<class C, class M>
		void setMethodListener( C * _self, M _method )
		{
			FactoryListenerInterfacePtr listener = new MethodFactoryListenerInterface<C, M, T>(_self, _method);

			m_factoryNode.setListener( listener );
		}

	protected:
		class FIntrusiveTreeForeachDestroy
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
}
#	pragma once

#	include "Factory/FactoryStore.h"

#	include <stdex/intrusive_splay_tree.h>

namespace Menge
{
	template<class T, size_t Count>
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
}
#	pragma once

#	include "Factory/FactoryStore.h"

#	include <stdex/intrusive_splay_tree.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		template<class T>
		class FIntrusiveSprayTreeForeachDestroy
		{
		public:
			void operator() ( T * _entry )
			{
				_entry->destroy();
			}
		};
	}

	template<class T, size_t Count>
	class IntrusiveSprayTree
		: public stdex::intrusive_splay_tree<T>
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
			FIntrusiveSprayTreeForeachDestroy<T> fistfd;
			this->foreach( fistfd );

			stdex::intrusive_splay_tree<T>::clear();
		}
				
	protected:
		typedef FactoryPoolStore<T, Count> TFactoryNode;
		TFactoryNode m_factoryNode;
	};
}
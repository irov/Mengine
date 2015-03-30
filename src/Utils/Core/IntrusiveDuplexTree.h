#	pragma once

#	include "Factory/FactoryStore.h"

#	include <stdex/intrusive_duplex_tree.h>

namespace Menge
{
	template<class T, uint32_t Count>
	class IntrusiveDuplexTree
		: public stdex::intrusive_duplex_tree<T>
	{
	public:
		typedef typename stdex::intrusive_duplex_tree<T>::key_first_type key_first_type;
		typedef typename stdex::intrusive_duplex_tree<T>::key_second_type key_second_type;

	public:
		IntrusiveDuplexTree()
		{
		}

		~IntrusiveDuplexTree()
		{
			this->clear();
		}

	public:
		T * create()
		{
			T * new_node = m_factoryNode.createObjectT();

			return new_node;
		}

		bool create_if( const key_first_type & _key1, const key_second_type & _key2, T ** _node )
		{
			T * exist_node;
			if( this->has( _key1, _key2, &exist_node ) == true )
			{
				*_node = exist_node;

				return false;
			}

			T * new_node = this->create();

			*_node = new_node;

			return true;
		}

		void clear()
		{
			FIntrusiveTreeForeachDestroy fistfd;
			this->foreach( fistfd );

			stdex::intrusive_duplex_tree<T>::clear();
		}

		void erase_node( T * _node )
		{
			stdex::intrusive_duplex_tree<T>::erase_node( _node );

			_node->destroy();
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
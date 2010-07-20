#	pragma once

#	include "IntrusiveLinked.h"

namespace Menge
{
	template<class I, class F>
	I intrusive_find_if( I _begin, I _end, F _pred )
	{
		for( I
			it = _begin,
			it_end = _end;
		it != it_end;
		++it )
		{
			if( _pred( *it ) == true )
			{
				return it;
			}
		}

		return _end;
	}

	template<class I, class T>
	I intrusive_find( I _begin, I _end, T * _node )
	{
		I it_find(_node);

		for( I
			it = _begin,
			it_end = _end;
		it != it_end;
		++it )
		{
			if( it == it_find )
			{
				return it;
			}
		}

		return _end;
	}

	template<class T>
	class IntrusiveList
	{
		typedef IntrusiveLinked TLinked;

	public:
		IntrusiveList()
		{
			m_head.m_right = &m_head;
			m_head.m_left = &m_head;
		}

		~IntrusiveList()
		{
			for( iterator
				it = begin(),
				it_end = end();
			it != it_end;
			++it )
			{
				TLinked * node = it.get();
				node->unlink();
			}

			m_head.m_right = 0;
			m_head.m_left = 0;
		}

	public:

		class const_iterator
		{
		public:
			const_iterator( TLinked * _node )
				: m_node(_node)
			{
			}

		public:
			T * operator -> () const
			{
				return static_cast<T*>(m_node);
			}

			T * operator * () const
			{
				return static_cast<T *>(m_node);
			}

			TLinked * get() const
			{
				return m_node;
			}

		public:
			const_iterator & operator ++ ()
			{
				m_node = m_node->m_right;

				return *this;
			}

			const_iterator operator ++ (int)
			{
				const_iterator tmp = *this;
				++*this;
				return tmp;
			}

			const_iterator & operator -- ()
			{
				m_node = m_node->m_left;

				return *this;
			}

			const_iterator operator -- ( int )
			{
				const_iterator tmp = *this;
				--*this;
				return tmp;
			}

			bool operator == ( const_iterator _it ) const
			{
				return m_node == _it.m_node;
			}

			bool operator != ( const_iterator _it ) const
			{
				return !(*this == _it ); 
			}

		protected:
			TLinked * m_node;
		};

		class iterator
			: public const_iterator
		{
		public:
			iterator()
				: m_node()
			{
			}

			iterator( TLinked * _node )
				: const_iterator(_node)
			{
			}

		public:
			T * operator -> ()
			{
				return static_cast<T*>(m_node);
			}

			T * operator * ()
			{
				return static_cast<T*>(m_node);
			}

			TLinked * get()
			{
				return m_node;
			}

		public:
			iterator & operator = ( const iterator & _it )
			{
				m_node = _it.m_node;

				return *this;
			}

			iterator & operator ++ ()
			{
				m_node = m_node->m_right;

				return *this;
			}

			iterator operator ++ (int)
			{
				iterator tmp = *this;
				++*this;
				return tmp;
			}

			iterator & operator -- ()
			{
				m_node = m_node->m_left;

				return *this;
			}

			iterator operator -- ( int )
			{
				iterator tmp = *this;
				--*this;
				return tmp;
			}

			bool operator == ( iterator _it )
			{
				return m_node == _it.m_node; 
			}

			bool operator != ( iterator _it )
			{
				return !this->operator == ( _it ); 
			}
		};

		class reverse_iterator
		{
		public:
			reverse_iterator( TLinked * _node )
				: m_node(_node)
			{
			}

			reverse_iterator( const reverse_iterator & _it )
				: m_node(_it.m_node)
			{
			}

		public:
			T * operator -> () const
			{
				return static_cast<T*>(m_node);
			}

			T * operator * () const
			{
				return static_cast<T *>(m_node);
			}

		public:
			reverse_iterator & operator ++ ()
			{
				m_node = m_node->m_left;

				return *this;
			}

			reverse_iterator operator ++ (int)
			{
				reverse_iterator tmp = *this;
				--*this;
				return tmp;
			}

			reverse_iterator & operator -- () const
			{
				m_node = m_node->m_right;

				return *this;
			}

			reverse_iterator operator -- ( int ) const
			{
				reverse_iterator tmp = *this;
				++*this;
				return tmp;
			}

			bool operator == ( reverse_iterator _it ) const
			{
				return m_node == _it.m_node;
			}

			bool operator != ( reverse_iterator _it ) const
			{
				return !(*this == _it ); 
			}

		protected:
			TLinked * m_node;
		};

	public:
		iterator begin()
		{
			return iterator(m_head.m_right);
		}

		iterator end()
		{
			return iterator(&m_head);
		}

		const_iterator begin() const
		{
			return const_iterator(m_head.m_right);
		}

		const_iterator end() const
		{
			return const_iterator(&m_head);
		}

		reverse_iterator rbegin()
		{
			return reverse_iterator(m_head.m_left);
		}

		reverse_iterator rend()
		{
			return reverse_iterator(&m_head);
		}

	public:
		T * front()
		{
			return *begin();
		}

		T * back()
		{
			return *end();
		}


	public:
		void push_front( IntrusiveLinked * _node )
		{	// insert element at beginning
			insert_( begin(), _node );
		}

		void pop_front()
		{	// erase element at beginning
			erase( begin() );
		}

		void push_back( IntrusiveLinked * _node )
		{	// insert element at end
			insert_( end(), _node );
		}

		void pop_back()
		{	// erase element at end
			erase( --end() );
		}

		void splice( iterator _from, iterator _where )
		{
			erase( _from );
			insert( --_where, *_from );
		}

		void clear()
		{
			iterator it = begin();
			iterator it_end = end();
			while( it != it_end )
			{
				it = erase( it );
			}
		}

		bool empty() const
		{
			return m_head.m_right == m_head.m_left;
		}

		iterator insert( iterator _where, IntrusiveLinked * _node )
		{	// insert _Val at _Where
			insert_( _where, _node );
			return (--_where);
		}

		void insert( iterator _where, iterator _begin, iterator _end )
		{	// insert _Val at _Where
			for(; _begin != _end; ++_begin )
			{
				insert( _where++, *_begin );
			}
		}

		iterator erase( iterator _where )
		{	// erase element at _Where
			iterator it = _where++;

			if ( it != end() )
			{	// not list head, safe to erase
				it->unlink();
			}

			return (_where);
		}

		template<class F>
		iterator erase_if( F _pred )
		{
			iterator it_found = find_if( _pred );

			if( it_found == end() )
			{
				return end();
			}

			return erase( it_found );			
		}

	protected:
		void insert_( iterator _where, IntrusiveLinked * _node )
		{	// insert _Val at _Where
			_where->link_before( _node );
		}

	protected:
		mutable IntrusiveLinked m_head;
	};
}
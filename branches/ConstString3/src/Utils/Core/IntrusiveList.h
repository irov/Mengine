#	pragma once

#	include "IntrusiveLinked.h"
#	include "IntrusiveAlgorithm.h"

namespace Menge
{
	template<class T>
	class IntrusiveList
	{
		typedef IntrusiveLinked TLinked;

	public:
		IntrusiveList()
			: m_head(EILT_END)
		{
			m_head.m_right = &m_head;
			m_head.m_left = &m_head;
		}

		~IntrusiveList()
		{
			this->clear();

			m_head.m_right = 0;
			m_head.m_left = 0;
		}

	protected:
		template<class It>
		class base_iterator
		{
		public:
			base_iterator( TLinked * _node )
				: m_node(_node)
			{
				while( m_node->isTag( EILT_SLUG ) == true )
				{
					m_node = m_node->right();
				}
			}

		public:
			inline T * operator -> () const
			{
				return static_cast<T *>(m_node);
			}

			inline T * operator * () const
			{
				return static_cast<T *>(m_node);
			}

			inline TLinked * get() const
			{
				return m_node;
			}

		public:
			bool operator == ( It _it ) const
			{
				return m_node == _it.m_node;
			}

			bool operator != ( It _it ) const
			{
				return !this->operator == ( _it );
			}

		protected:
			inline void shuffle_next()
			{
				do
				{
					m_node = m_node->right();
				}
				while( m_node->isTag(EILT_SLUG) == true );
			}

			inline void shuffle_prev()
			{
				do
				{
					m_node = m_node->left();
				}
				while( m_node->isTag(EILT_SLUG) == true );			
			}


		protected:
			TLinked * m_node;
		};

	public:
		class const_iterator
			: public base_iterator<const_iterator>
		{
		public:
			const_iterator( TLinked * _node )
				: base_iterator<const_iterator>(_node)
			{
			}

		public:
			const_iterator & operator = ( const const_iterator & _it )
			{
				this->m_node = _it.m_node;

				return *this;
			}

		public:
			const_iterator & operator ++ ()
			{
				this->shuffle_next();

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
				this->shuffle_prev();

				return *this;
			}

			const_iterator operator -- ( int )
			{
				const_iterator tmp = *this;
				--*this;
				return tmp;
			}
		};

		class iterator
			: public base_iterator<iterator>
		{
		public:
			iterator( TLinked * _node )
				: base_iterator<iterator>(_node)
			{
			}

		public:
			iterator & operator = ( const iterator & _it )
			{
				this->m_node = _it.m_node;

				return *this;
			}

			iterator & operator ++ ()
			{
				this->shuffle_next();

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
				this->shuffle_prev();

				return *this;
			}

			iterator operator -- ( int )
			{
				iterator tmp = *this;
				--*this;
				return tmp;
			}
		};

		class reverse_iterator
			: public base_iterator<reverse_iterator>
		{
		public:
			reverse_iterator( TLinked * _node )
				: base_iterator<reverse_iterator>(_node)
			{
			}

		public:
			reverse_iterator & operator ++ ()
			{
				this->shuffle_prev();

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
				this->shuffle_next();

				return *this;
			}

			reverse_iterator operator -- ( int ) const
			{
				reverse_iterator tmp = *this;
				++*this;
				return tmp;
			}
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
		T * front() const
		{
			return *this->begin();
		}

		T * back() const
		{
			return *this->end();
		}

	public:
		void push_front( IntrusiveLinked * _node )
		{	
			this->insert_( this->begin(), _node );
		}

		void pop_front()
		{	
			this->erase( this->begin() );
		}

		void push_back( IntrusiveLinked * _node )
		{	
			this->insert_( this->end(), _node );
		}

		void pop_back()
		{	
			this->erase( --this->end() );
		}

		void splice( iterator _from, iterator _where )
		{
			this->erase( _from );
			this->insert( --_where, *_from );
		}

		void clear()
		{
			iterator it = this->begin();
			iterator it_end = this->end();
			while( it != it_end )
			{
				it = this->erase( it );
			}
		}

		size_t size() const
		{
			size_t count = intrusive_distance( this->begin(), this->end() );

			return count;
		}

		bool empty() const
		{
			return m_head.m_right == &m_head;
		}

		iterator insert( iterator _where, IntrusiveLinked * _node )
		{	
			this->insert_( _where, _node );

			return (--_where);
		}

		void insert( iterator _where, iterator _begin, iterator _end )
		{	
			if( _begin == _end )
			{
				return;
			}

			_end->m_left->m_right = 0;
			_end->m_left = _begin->m_left;

			_begin->m_left->m_right = *_end;
			_begin->m_left = 0;

			_begin->linkall( *_where );
		}

		iterator erase( iterator _where )
		{	
			iterator it = _where++;

			if( it != this->end() )
			{	
				TLinked * node = it.get();
				node->unlink();
			}

			return (_where);
		}

		template<class F>
		iterator erase_if( F _pred )
		{
			iterator it_found = find_if( _pred );

			if( it_found == this->end() )
			{
				return this->end();
			}

			return this->erase( it_found );			
		}

	protected:
		void insert_( iterator _where, IntrusiveLinked * _node )
		{	
			_where->link_before( _node );
		}

	protected:
		mutable IntrusiveLinked m_head;
	};
}
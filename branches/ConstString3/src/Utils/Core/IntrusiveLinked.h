#	pragma once

namespace Menge
{
	class IntrusiveLinked
	{
		typedef IntrusiveLinked TPtr;

	public:
		IntrusiveLinked()
			: m_right(0)
			, m_left(0)
		{
		}

		~IntrusiveLinked()
		{
			this->unlink();
		}

	public:
		bool unique() const
		{
			return !m_right && !m_left;
		}

		TPtr * left()
		{
			return m_left;
		}

		TPtr * right()
		{
			return m_right;
		}

		void link_after( TPtr * _other )
		{
			_other->m_right = m_right;
			_other->m_left = (this);

			if( m_right )
			{
				m_right->m_left = _other;
			}

			m_right = _other;
		}

		void link_before( TPtr * _other )
		{
			_other->m_left = m_left;
			_other->m_right = (this);

			if( m_left )
			{
				m_left->m_right = _other;
			}

			m_left = _other;
		}


		void unlink()
		{
			if( m_right )
			{
				m_right->m_left = m_left;
			}

			if( m_left )
			{
				m_left->m_right = m_right;
			}
			
			m_left = 0;
			m_right = 0;
		}

		TPtr * leftcast() const
		{
			TPtr * it = m_left;

			while( it->m_left )
			{
				it = it->m_left;
			}

			return it;
		}

		TPtr * rightcast() const
		{
			TPtr * it = m_right;

			while( it->m_right )
			{
				it = it->m_right;
			}

			return it;
		}

		void linkall( TPtr * _other )
		{
			TPtr * other_right = _other->m_right;

			if( m_left )
			{
				TPtr * left = leftcast();

				left->m_left = _other;
				_other->m_right = left;
			}
			else
			{
				m_left = _other;
				_other->m_right = (this);
			}

			if( m_right )
			{
				if( other_right )
				{
					TPtr * right = rightcast();

					other_right->m_left = right;
					right->m_right = other_right;
				}
			}
			else
			{
				if( other_right )
				{
					other_right->m_left = (this);
					m_right = other_right;
				}
			}
		}

		template<class F>
		void foreach( F _f ) const
		{
			foreach_self<F>( _f );
			foreach_other<F>( _f );
		}

		template<class F>
		void foreach_self( F _f ) const
		{
			const TPtr * nc_this = (this);
			_f( const_cast<TPtr *>(nc_this) );
		}

		template<class F>
		void foreach_other( F _pred ) const
		{
			TPtr * it_right = m_right;
			while( it_right )
			{
				_pred( it_right );
				it_right = it_right->m_right;
			}

			TPtr * it_left = m_left;
			while( it_left )
			{
				_pred( (it_left) );
				it_left = it_left->m_left;
			}
		}

		template<class F>
		TPtr * find( F _pred ) const
		{
			TPtr * node_found = find_self( _pred );

			if( node_found )
			{
				return node_found;
			}

			return find_other( _pred );
		}

		template<class F>
		TPtr * find_self( F _pred ) const
		{
			if( _pred(this) == true )
			{
				return this;
			}

			return 0;
		}

		template<class F>
		TPtr * find_other( F _pred ) const
		{
			TPtr * it_right = m_right;
			while( it_right )
			{
				if( _pred( it_right ) )
				{
					return this;
				}

				it_right = it_right->m_right;
			}

			TPtr * it_left = m_left;
			while( it_left )
			{
				if( _pred( it_left ) )
				{
					return this;
				}

				it_left = it_left->m_left;
			}

			return 0;
		}

	public:
		mutable TPtr * m_right;
		mutable TPtr * m_left;
	};
}
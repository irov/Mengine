#	pragma once

namespace Menge
{
	enum EIntrusiveLinkedTag
	{
		EILT_ELEMENT,
		EILT_END,
		EILT_SLUG
	};

    template<class Tag>
	class IntrusiveLinked
	{
		typedef IntrusiveLinked<Tag> TPtr;

	public:
		inline IntrusiveLinked()
			: m_right(0)
			, m_left(0)
			, m_tag(EILT_ELEMENT)
		{
		}

        inline IntrusiveLinked( EIntrusiveLinkedTag _tag )
			: m_right(0)
			, m_left(0)
			, m_tag(_tag)
		{
		}

		inline ~IntrusiveLinked()
		{
			this->unlink();
		}

    private:
        IntrusiveLinked & operator = ( const IntrusiveLinked & _linked )
        {
            (void)_linked;

            return *this;
        }

	public:
		inline int getIntrusiveTag() const
		{ 
			return m_tag;
		}

		inline bool unique() const
		{
			return !m_right && !m_left;
		}

		inline TPtr * left() const
		{
			return m_left;
		}

		inline TPtr * right() const
		{
			return m_right;
		}

		inline void link_after( TPtr * _other )
		{
			_other->m_right = m_right;
			_other->m_left = (this);

			if( m_right )
			{
				m_right->m_left = _other;
			}

			m_right = _other;
		}

		inline void link_before( TPtr * _other )
		{
			_other->m_left = m_left;
			_other->m_right = (this);

			if( m_left )
			{
				m_left->m_right = _other;
			}

			m_left = _other;
		}


		inline void unlink()
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

		inline TPtr * leftcast() const
		{
			TPtr * it = m_left;

			while( it->m_left )
			{
				it = it->m_left;
			}

			return it;
		}

		inline TPtr * rightcast() const
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
				TPtr * left = this->leftcast();

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
					TPtr * right = this->rightcast();

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
		const TPtr * find( F _pred ) const
		{
			TPtr * node_found = this->find_self( _pred );

			if( node_found )
			{
				return node_found;
			}

			return find_other( _pred );
		}

		template<class F>
		const TPtr * find_self( F _pred ) const
		{
			if( _pred(this) == true )
			{
				return this;
			}

			return 0;
		}

		template<class F>
		const TPtr * find_other( F _pred ) const
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
		int m_tag;
	};
}
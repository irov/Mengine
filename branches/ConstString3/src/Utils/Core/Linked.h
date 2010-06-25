#	pragma once

namespace Menge
{
	template<class T, class F>
	T * intrusive_linked_cast( class intrusive_linked * _linked );

	template<class T>
	class intrusive_linked
	{
	public:
		intrusive_linked()
			: m_right(0)
			, m_left(0)
		{
		}

		~intrusive_linked()
		{
			this->unlink();
		}

	public:
		bool empty() const
		{
			return !m_right && !m_left;
		}

		void link( T * _other )
		{
			m_right = _other->m_right;
			m_left = _other;

			if( m_left )
			{
				m_left->m_right = intrusive_linked_cast<T>(this);
			}

			if( m_right )
			{
				m_right->m_left = intrusive_linked_cast<T>(this);
			}
		}

		void unlink()
		{
			if( m_right )
			{
				m_right->m_left = m_left;

				if( m_right->empty() )
				{
					intrusive_linked_unlick<T>(this);
				}
			}

			if( m_left )
			{
				m_left->m_right = m_right;

				if( m_left->empty() )
				{
					intrusive_linked_unlick<T>(this);
				}
			}
		}

		T * leftcast()
		{
			T * it = m_left;

			while( it->m_left )
			{
				it = it->m_left;
			}

			return it;
		}

		T * rightcast()
		{
			T * it = m_right;

			while( it->m_right )
			{
				it = it->m_right;
			}

			return it;
		}

		void linkall( T * _other )
		{
			if( m_left )
			{
				T * left = leftcast();
				left->m_left = _other;
				_other->m_right = left;
			}
			else
			{
				m_left = _other;
				_other->m_right = intrusive_linked_cast<T>(this);
			}

			if( m_right )
			{
				T * right = rightcast();

				if( _other->m_right )
				{
					_other->m_right->m_left = right;
				}
			}
			else
			{
				if( _other->m_right )
				{
					_other->m_right->m_left = intrusive_linked_cast<T>(this);
				}

				m_right = _other->m_right;
			}
		}

		template<class F>
		void foreach( F _f )
		{
			T * it_right = m_right;
			while( it_right )
			{
				_f(it_right);
				it_right = it_right->m_right;
			}

			T * it_left = m_left;
			while( it_left )
			{
				_f(it_left);
				it_left = it_left->m_left;
			}
		}

	private:
		mutable T * m_right;
		mutable T * m_left;
	};
}
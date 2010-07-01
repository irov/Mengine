#	pragma once

namespace Menge
{
	template<class T>
	class IntrusiveLinked
	{
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
				m_left->m_right = static_cast<T>(this);
			}

			if( m_right )
			{
				m_right->m_left = static_cast<T>(this);
			}
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
			T * other_right = _other->m_right;

			if( m_left )
			{
				T * left = leftcast();

				left->m_left = _other;
				_other->m_right = left;
			}
			else
			{
				m_left = _other;
				_other->m_right = static_cast<T*>(this);
			}

			if( m_right )
			{
				if( other_right )
				{
					T * right = rightcast();

					other_right->m_left = right;
					right->m_right = other_right;
				}
			}
			else
			{
				if( other_right )
				{
					other_right->m_left = static_cast<T*>(this);
					m_right = other_right;
				}
			}
		}

		template<class F>
		void foreach( F _f )
		{
			_f( static_cast<T*>(this) );

			T * it_right = m_right;
			while( it_right )
			{
				_f( static_cast<T*>(it_right) );
				it_right = it_right->m_right;
			}

			T * it_left = m_left;
			while( it_left )
			{
				_f( static_cast<T*>(it_left) );
				it_left = it_left->m_left;
			}
		}

	private:
		mutable T * m_right;
		mutable T * m_left;
	};
}
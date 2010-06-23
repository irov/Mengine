#	include <boost/intrusive_ptr.hpp>

#	include <string>

namespace Menge
{
	namespace Detail
	{
		struct ConstStringHolder
		{
		public:
			ConstStringHolder( const std::string & _value )
				: m_value(_value)
				, m_reference(0)
			{
			}

		public:
			const std::string & str() const
			{
				return m_value;
			}

		public:
			std::size_t reference() const
			{
				return m_reference;
			}

		protected:
			std::size_t m_reference;
			std::string m_value;

			friend inline void intrusive_ptr_add_ref( ConstStringHolder * _ptr )
			{
				++_ptr->m_reference;
			}

			friend inline void intrusive_ptr_release( ConstStringHolder * _ptr )
			{
				if( --_ptr->m_reference == 0 )
				{
					delete _ptr;
				}
			}

		};

		typedef boost::intrusive_ptr<ConstStringHolder> ConstStringHolderPtr;
	}

	struct ConstString
	{
	public:
		ConstString( const char * _str )
			: m_holder( new Detail::ConstStringHolder(_str) )
			, m_next(0)
			, m_prev(0)
		{
		}

		ConstString( const std::string & _str )
			: m_holder( new Detail::ConstStringHolder(_str) )
			, m_next(0)
			, m_prev(0)
		{
		}

		ConstString( const ConstString & _cstr )
			: m_holder(_cstr.m_holder)
		{
			this->link( _cstr );
		}

		~ConstString()
		{
			this->unlink();
		}

	public:
		const char * c_str() const
		{
			return m_holder->str().c_str();
		}

	public:
		ConstString & operator = ( const ConstString & _cstr )
		{
			if( m_holder == _cstr.m_holder )
			{
				return *this;
			}

			if( m_holder == 0 )
			{
				m_holder = _cstr.m_holder;
				this->link(_cstr);
			}
			else if( m_holder->str() == _cstr.m_holder->str() )
			{
				this->combine( _cstr );
			}
			else
			{
				this->unlink();
				m_holder = _cstr.m_holder;
				this->link(_cstr);
			}

			return *this;
		}

		bool operator == ( const ConstString & _cstr ) const
		{
			if( m_holder == _cstr.m_holder )
			{
				return true;
			}

			if( m_holder->str() != _cstr.m_holder->str() )
			{
				return false;
			}

			this->combine( _cstr );

			return true;
		}

		bool operator != ( const ConstString & _cstr ) const
		{
			return !this->operator == (_cstr);
		}

		friend bool operator < ( const ConstString & _left, const ConstString & _right )
		{
			if( _left.m_holder == _right.m_holder )
			{
				return false;
			}

			int res = _left.m_holder->str().compare( _right.m_holder->str() );

			if( res == 0 )
			{
				_left.combine( _right );

				return false;
			}

			return res < 0;
		}

		void link( const ConstString & _cstr ) const
		{
			if( _cstr.m_next )
			{
				_cstr.m_next->m_prev = this;
				m_next = _cstr.m_next;
			}
			else
			{
				m_next = 0;
			}

			m_prev = &_cstr;
			_cstr.m_next = this;
		}

		void unlink() const
		{
			if( m_next )
			{
				m_next->m_prev = m_prev;
			}

			if( m_prev )
			{
				m_prev->m_next = m_next;
			}
		}

		void combine( const ConstString & _cstr ) const
		{
			if( m_holder->reference() < _cstr.m_holder->reference() )
			{
				m_holder = _cstr.m_holder;

				upfill( m_next, _cstr.m_holder );
				downfill( m_prev, _cstr.m_holder );

				linkchain( &_cstr, this );
			}
			else
			{
				_cstr.m_holder = m_holder;

				upfill( _cstr.m_next, m_holder );
				downfill( _cstr.m_prev, m_holder );

				linkchain( this, &_cstr );
			}
		}

		friend const ConstString * downcast( const ConstString * _it )
		{
			while( _it->m_prev )
			{
				_it = _it->m_prev;
			}

			return _it;
		}

		friend const ConstString * upcast( const ConstString * _it )
		{
			while( _it->m_next )
			{
				_it = _it->m_next;
			}

			return _it;
		}

		friend void upfill( const ConstString * _it, const Detail::ConstStringHolderPtr & _holder )
		{
			while( _it )
			{
				_it->m_holder = _holder;
				_it = _it->m_next;
			}
		}

		friend void downfill( const ConstString * _it, const Detail::ConstStringHolderPtr & _holder )
		{
			while( _it )
			{
				_it->m_holder = _holder;
				_it = _it->m_prev;
			}
		}

		friend void linkchain( const ConstString * _left, const ConstString * _chain )
		{
			const ConstString * middle = _chain;
			const ConstString * front = downcast( middle );
			const ConstString * back = upcast( middle );

			front->m_prev = _left;

			if( _left->m_next )
			{
				_left->m_next->m_prev = back;
			}

			back->m_next = _left->m_next;
			_left->m_next = front;
		}

	public:
		mutable Detail::ConstStringHolderPtr m_holder;
		mutable const ConstString * m_next;
		mutable const ConstString * m_prev;
	};
}
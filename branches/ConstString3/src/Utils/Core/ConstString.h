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
				, m_owner(0)
			{
			}

		public:
			const std::string & str() const
			{
				if( m_owner )
				{
					return m_owner->str();
				}

				return m_value;
			}

		public:
			std::size_t reference() const
			{
				return m_reference;
			}

		public:
			bool equal( const ConstStringHolder * _holder )
			{
				if( m_owner )
				{
					if( _holder->m_owner )
					{
						return m_owner == _holder->m_owner;
					}
					else
					{
						return m_owner == _holder;
					}
				}
				else
				{
					if( _holder->m_owner )
					{
						return this == _holder->m_owner;
					}
				}

				return this == _holder;
			}

		public:
			void combine( ConstStringHolder * _ptr )
			{
				std::string empty;
				if( m_reference > _ptr->reference() )
				{
					_ptr->m_value.swap(empty);
					_ptr->m_owner = this;
				}
				else
				{
					m_value.swap(empty);
					m_owner = _ptr;
				}
			}

		protected:
			std::size_t m_reference;
			std::string m_value;

			ConstStringHolder * m_owner;

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
		{
		}

		ConstString( const std::string & _str )
			: m_holder( new Detail::ConstStringHolder(_str) )
		{
		}

		ConstString( const ConstString & _cstr )
			: m_holder(_cstr.m_holder)
		{
		}

		~ConstString()
		{
		}

	public:
		const char * c_str() const
		{
			return m_holder->str().c_str();
		}

	public:
		ConstString & operator = ( const ConstString & _cstr )
		{
			if( m_holder->equal( _cstr.m_holder.get() ) == true )
			{
				return *this;
			}

			if( m_holder && m_holder->str() == _cstr.m_holder->str() )
			{
				this->combine( _cstr );
			}
			else
			{
				m_holder = _cstr.m_holder;
			}

			return *this;
		}

		bool operator == ( const ConstString & _cstr ) const
		{
			if( m_holder->equal( _cstr.m_holder.get() ) == true )
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
			if( _left.m_holder->equal( _right.m_holder.get() ) == true )
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

		void combine( const ConstString & _cstr ) const
		{
			m_holder->combine( _cstr.m_holder.get() );
		}

	public:
		Detail::ConstStringHolderPtr m_holder;
	};
}
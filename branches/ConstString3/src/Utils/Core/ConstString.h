#	pragma once

#	include "IntrusivePtr.h"
#	include "IntrusiveLinked.h"

#	include <string>
#	include <vector>
#	include <algorithm>

namespace Menge
{
	namespace Detail
	{
		struct ConstStringHolder
			: public intrusive_linked<ConstStringHolder>
		{
		public:
			ConstStringHolder( const std::string & _value )
				: m_value(_value)
				, m_reference(0)
				, m_owner(this)
			{
				hash();
			}

			~ConstStringHolder()
			{
			}

		public:
			const std::string & str() const
			{
				return m_owner->m_value;
			}

		public:
			std::size_t reference() const
			{
				return m_reference;
			}

			ConstStringHolder * owner() const
			{
				return m_owner;
			}

		public:
			bool less( ConstStringHolder * _holder )
			{
				if( m_lesshash < _holder->m_lesshash )
				{
					return true;
				}
				else if( m_lesshash > _holder->m_lesshash )
				{
					return false;
				}

				if( m_owner == _holder->m_owner )
				{
					return false;
				}

				int res = str().compare( _holder->str() );

				if( res == 0 )
				{
					this->combine( _holder );
				}

				return res < 0;
			}

			bool equal( ConstStringHolder * _holder )
			{
				if( m_owner == _holder->m_owner )
				{
					return true;
				}

				if( str() != _holder->str() )
				{
					return false;
				}

				this->combine( _holder );

				return true;
			}

		public:
			void combine( ConstStringHolder * _holder )
			{
				if( m_reference > _holder->m_reference )
				{
					combine_from( this, _holder );
				}
				else
				{
					combine_from( _holder, this );
				}
			}

		protected:
			struct ForeachCombineOwner
			{
				ForeachCombineOwner( ConstStringHolder * _owner )
					: m_owner(_owner)
				{
				}

				void operator () ( intrusive_linked * _elem )
				{
					static_cast<ConstStringHolder *>(_elem)->m_owner = m_owner;
				}

				ConstStringHolder * m_owner;
			};

			void combine_from( ConstStringHolder * _out, ConstStringHolder * _from )
			{
				_from->m_owner = _out;
				_from->foreach( ForeachCombineOwner(_out) );
				_from->linkall( _out );

				{
					std::string empty;
					_from->m_value.swap(empty);
				}
			}

		protected:
			void hash()
			{
				std::string::size_type size = m_value.size();

				std::string::size_type size_4 = size >> 2;

				m_lesshash = 0;

				if( size_4 == 0 )
				{
					for( std::string::size_type it = 0; it != size; ++it )
					{
						m_lesshash += m_value[it] << (it << 3);
					}
				}
				else if( size_4 == 1 )
				{
					m_lesshash = *(std::size_t *)&m_value[size-size_4];
				}
				else
				{
					for( std::string::size_type it = 0; it != 4; ++it )
					{
						unsigned char i = 0;
						for( std::string::size_type it_i = 0; it_i != size_4; ++it_i )
						{
							i += m_value[it];
						}

						m_lesshash = i << (it << 3);
					}
				}
			}

		protected:			
			std::string m_value;
			std::size_t m_reference;

			std::size_t m_lesshash;

			ConstStringHolder * m_owner;

		protected:
			friend inline void intrusive_ptr_add_ref( ConstStringHolder * _ptr )
			{
				++_ptr->m_reference;
			}

			friend inline void intrusive_ptr_release( ConstStringHolder * _ptr )
			{
				if( --_ptr->m_reference == 0 && _ptr->empty() )
				{					
					delete _ptr;
				}
			}

			friend inline void intrusive_linked_unlick( ConstStringHolder * _holder )
			{

			}
		};

		typedef IntrusivePtr<ConstStringHolder> ConstStringHolderPtr;
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
			: m_holder(_cstr.m_holder->owner())
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

			if( m_holder->str() == _cstr.m_holder->str() )
			{
				this->combine( _cstr );
			}
			else
			{
				m_holder = _cstr.m_holder->owner();
			}

			return *this;
		}

		bool operator == ( const ConstString & _cstr ) const
		{
			return m_holder->equal( _cstr.m_holder.get() );
		}

		bool operator != ( const ConstString & _cstr ) const
		{
			return !this->operator == (_cstr);
		}

		friend bool operator < ( const ConstString & _left, const ConstString & _right )
		{
			return _left.m_holder->less( _right.m_holder.get() );
		}

		void combine( const ConstString & _cstr ) const
		{
			m_holder->combine( _cstr.m_holder.get() );
		}

	public:
		Detail::ConstStringHolderPtr m_holder;
	};
}
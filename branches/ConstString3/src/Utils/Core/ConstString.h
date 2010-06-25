#	pragma once

#	include <boost/intrusive_ptr.hpp>

#	include "Linked.h"

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
			std::string m_value;
			std::size_t m_reference;

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
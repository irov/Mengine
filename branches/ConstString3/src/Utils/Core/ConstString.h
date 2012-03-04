#	pragma once

#	include "IntrusivePtr.h"
#	include "IntrusiveLinked.h"

#	include <string>

namespace Menge
{
#	define MENGE_CONST_STRING

#	if defined(MENGE_CONST_STRING)
	namespace Detail
	{
		struct ConstStringHolder
			: public IntrusiveLinked
		{
		public:
			ConstStringHolder( const std::string & _value );
			ConstStringHolder( char * _str );

			ConstStringHolder( const char * _str, size_t _size );

		public:			
			inline const std::string & str() const
			{
				return m_owner->m_value;
			}

		public:
			inline ConstStringHolder * owner() const
			{
				return m_owner;
			}

		public:
			bool less( ConstStringHolder * _holder );
			bool equal( ConstStringHolder * _holder );

		public:
			void combine( ConstStringHolder * _holder );

		protected:
			void release_string();

			class ForeachCombineOwner;
			class ForeachCombineOther;

			void combine_owner( ConstStringHolder * _out );
			void combine_other( ConstStringHolder * _out );
			
			void combine_from( ConstStringHolder * _from, ConstStringHolder * _out );

			void hash();		

		protected:
			friend inline void intrusive_ptr_add_ref( ConstStringHolder * _ptr )
			{
				++_ptr->m_reference;
				++_ptr->m_owner->m_reference;
			}

			friend inline void intrusive_ptr_release( ConstStringHolder * _ptr )
			{
				if( --_ptr->m_owner->m_reference == 0 )
				{
					delete _ptr->m_owner;
				}

				if( --_ptr->m_reference == 0 )
				{
					delete _ptr;
				}
			}

		protected:			
			std::string m_value;
			size_t m_reference;

			size_t m_lesshash;

			mutable ConstStringHolder * m_owner;
		};

		typedef IntrusivePtr<ConstStringHolder> ConstStringHolderPtr;
	}

	class ConstString
	{
	public:
		static ConstString none;

	public:
		ConstString();
		ConstString( const ConstString & _cstr );

	public:
		explicit ConstString( char * _str );
		explicit ConstString( const char * _str );
		explicit ConstString( const char * _str, size_t _size );
		explicit ConstString( const std::string & _str );

	public:
		inline const std::string & to_str() const
		{
			return m_holder->str();
		}

		inline size_t size() const
		{
			return to_str().size();
		}

		inline const char * c_str() const
		{
			return to_str().c_str();
		}

		inline bool empty() const
		{
			return to_str().empty();
		}

		inline void clear()
		{
			m_holder = none.m_holder->owner();
		}

	public:
		inline ConstString & operator = ( const ConstString & _cstr )
		{
			if( m_holder->equal( _cstr.m_holder.get() ) == false )
			{
				m_holder = _cstr.m_holder->owner();
			}

			return *this;
		}

		inline bool operator == ( const std::string & _str ) const
		{
			return m_holder->str() == _str;
		}

		inline bool operator != ( const std::string & _str ) const
		{
			return !this->operator == (_str);
		}

		inline bool operator == ( const char * _str ) const
		{
			return m_holder->str() == _str;
		}

		inline bool operator != ( const char * _str ) const
		{
			return !this->operator == (_str);
		}

		inline bool operator == ( const ConstString & _cstr ) const
		{
			return m_holder->equal( _cstr.m_holder.get() );
		}

		inline bool operator != ( const ConstString & _cstr ) const
		{
			return !this->operator == (_cstr);
		}

		inline friend bool operator < ( const ConstString & _left, const ConstString & _right )
		{
			return _left.m_holder->less( _right.m_holder.get() );
		}

	public:
		Detail::ConstStringHolderPtr m_holder;
	};

	namespace Helper
	{
		inline const std::string & to_str( const ConstString & _cs )
		{
			return _cs.to_str();
		}

		inline const ConstString & to_none()
		{
			return ConstString::none;
		}
	}

#	else
	typedef std::string ConstString;

	namespace Helper
	{
		inline const std::string & to_str( const ConstString & _cs )
		{
			return _cs;
		}

		inline const ConstString & to_none()
		{
			static ConstString none;
			return none;
		}
	}

#	endif
}
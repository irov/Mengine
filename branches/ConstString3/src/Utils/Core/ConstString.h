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
			inline ConstStringHolder( const std::string & _value )
				: m_value(_value)
				, m_reference(0)
			{
				m_owner = this;
				hash();
			}

			inline ConstStringHolder( const char * _str, std::size_t _size )
				: m_value(_str, _size)
				, m_reference(0)
			{
				m_owner = this;
				hash();
			}

		public:
			inline const std::string & str() const
			{
				return m_owner->m_value;
			}

		public:
			inline std::size_t reference() const
			{
				return m_reference;
			}

			inline ConstStringHolder * owner() const
			{
				return m_owner;
			}

		public:
			inline bool less( ConstStringHolder * _holder )
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
					return false;
				}

				return res < 0;
			}

			inline bool equal( ConstStringHolder * _holder )
			{
				if( m_owner == _holder->m_owner )
				{
					return true;
				}

				if( m_lesshash != _holder->m_lesshash )
				{
					return false;
				}

				if( str() != _holder->str() )
				{
					return false;
				}

				this->combine( _holder );

				return true;
			}

		public:
			inline void combine( ConstStringHolder * _holder )
			{
				if( m_owner->m_reference > _holder->m_owner->m_reference )
				{
					combine_from( _holder, this );
				}
				else
				{
					combine_from( this, _holder );
				}
			}

		protected:
			struct ForeachCombineOwner
			{
				inline ForeachCombineOwner( ConstStringHolder * _out )
					: m_out(_out)
				{
				}

				inline void operator () ( IntrusiveLinked * _linked )
				{
					ConstStringHolder * elem = static_cast<ConstStringHolder *>(_linked);
					elem->m_owner = m_out->m_owner;
					elem->m_reference >>= 1;
					m_out->m_owner->m_reference += elem->m_reference;
				}

				ConstStringHolder * m_out;
			};


			struct ForeachCombineOther
			{
				inline ForeachCombineOther( ConstStringHolder * _out )
					: m_out(_out)
				{
				}

				inline void operator () ( IntrusiveLinked * _linked )
				{
					ConstStringHolder * elem = static_cast<ConstStringHolder *>(_linked);
					elem->m_owner->m_reference -= elem->m_reference;
					elem->m_owner = m_out->m_owner;
					m_out->m_owner->m_reference += elem->m_reference;
				}

				ConstStringHolder * m_out;
			};

			inline void combine_from( ConstStringHolder * _from, ConstStringHolder * _out )
			{
				{
					std::string empty;
					_from->m_owner->m_value.swap(empty);
				}
				

				if( _from->unique() == true )
				{
					ForeachCombineOwner combineowner(_out);
					_from->foreach_self( combineowner );
				}
				else
				{
					ConstStringHolder * from_owner = _from->m_owner;

					ForeachCombineOther combineother(_out);
					from_owner->foreach_other( combineother );

					if( from_owner->m_reference == 0 )
					{
						delete from_owner;
					}
					else
					{
						ForeachCombineOwner combineowner(_out);
						from_owner->foreach_self( combineowner );						
					}
				}

				_from->linkall( _out );
			}

		protected:
			inline void hash()
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
					m_lesshash = *(std::size_t *)&m_value[size-4];
				}
				else
				{
					for( std::string::size_type it = 0; it != 4; ++it )
					{
						unsigned char i = 0;
						for( std::string::size_type it_i = 0; it_i != size_4; ++it_i )
						{
							i += m_value[it_i + it*size_4];
						}

						m_lesshash = i << (it << 3);
					}
				}
			}

		protected:			
			std::string m_value;
			std::size_t m_reference;

			std::size_t m_lesshash;

			mutable ConstStringHolder * m_owner;

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
		};

		typedef IntrusivePtr<ConstStringHolder> ConstStringHolderPtr;
	}

	class ConstString
	{
	public:
		static ConstString none;

	public:
		ConstString()
			: m_holder(none.m_holder)
		{
		}

		ConstString( const ConstString & _cstr )
			: m_holder(_cstr.m_holder->owner())
		{
		}

	public:
		inline explicit ConstString( const char * _str )
			: m_holder( new Detail::ConstStringHolder(_str) )
		{
		}

		inline explicit ConstString( const char * _str, std::size_t _size )
			: m_holder( new Detail::ConstStringHolder(_str, _size) )
		{
		}

		inline explicit ConstString( const std::string & _str )
			: m_holder( new Detail::ConstStringHolder(_str) )
		{
		}

	public:
		inline const std::string & to_str() const
		{
			return m_holder->str();
		}

		inline std::size_t size() const
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
			m_holder = none.m_holder;
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
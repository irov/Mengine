#	include "ConstString.h"

namespace Menge
{
#	if defined(MENGE_CONST_STRING)
	ConstString ConstString::none("");

	namespace Detail
	{
		//////////////////////////////////////////////////////////////////////////
		ConstStringHolder::ConstStringHolder( const std::string & _value )
			: m_value(_value)
			, m_reference(0)
		{
			m_owner = this;
			this->hash();
		}
		//////////////////////////////////////////////////////////////////////////
		ConstStringHolder::ConstStringHolder( char * _str )
			: m_value(_str)
			, m_reference(0)
		{
			m_owner = this;
			this->hash();
		}
		//////////////////////////////////////////////////////////////////////////
		ConstStringHolder::ConstStringHolder( const char * _str, size_t _size )
			: m_value(_str, _size)
			, m_reference(0)
		{
			m_owner = this;
			hash();
		}
		//////////////////////////////////////////////////////////////////////////
		bool ConstStringHolder::less( ConstStringHolder * _holder )
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

			const std::string & left = this->str();
			const std::string & right = _holder->str();

			int res = left.compare( right );

			if( res == 0 )
			{
				this->combine( _holder );

				return false;
			}

			return res < 0;
		}
		//////////////////////////////////////////////////////////////////////////
		bool ConstStringHolder::equal( ConstStringHolder * _holder )
		{
			if( m_owner == _holder->m_owner )
			{
				return true;
			}

			if( m_lesshash != _holder->m_lesshash )
			{
				return false;
			}

			const std::string & left = this->str();
			const std::string & right = _holder->str();

			if( left != right )
			{
				return false;
			}

			this->combine( _holder );

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		void ConstStringHolder::combine( ConstStringHolder * _holder )
		{
			if( m_owner->m_reference > _holder->m_owner->m_reference )
			{
				this->combine_from( _holder, this );
			}
			else
			{
				this->combine_from( this, _holder );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void ConstStringHolder::combine_owner( ConstStringHolder * _out )
		{
			this->m_owner = _out->m_owner;
			this->m_reference >>= 1;
			_out->m_owner->m_reference += this->m_reference;
		}
		//////////////////////////////////////////////////////////////////////////
		void ConstStringHolder::combine_other( ConstStringHolder * _out )
		{
			this->m_owner->m_reference -= this->m_reference;
			this->m_owner = _out->m_owner;
			_out->m_owner->m_reference += this->m_reference;
		}
		//////////////////////////////////////////////////////////////////////////
		void ConstStringHolder::release_string()
		{
			std::string empty;
			m_value.swap(empty);
		}
		//////////////////////////////////////////////////////////////////////////
		class ConstStringHolder::ForeachCombineOwner
		{
		public:
			ConstStringHolder::ForeachCombineOwner( ConstStringHolder * _out )
				: m_out(_out)
			{
			}

		public:
			void operator () ( IntrusiveLinked * _linked )
			{
				ConstStringHolder * elem = static_cast<ConstStringHolder *>(_linked);

				elem->combine_owner( m_out );
			}

		protected:
			ConstStringHolder * m_out;
		};
		//////////////////////////////////////////////////////////////////////////
		class ConstStringHolder::ForeachCombineOther
		{
		public:
			ConstStringHolder::ForeachCombineOther( ConstStringHolder * _out )
				: m_out(_out)
			{
			}

		public:
			void operator () ( IntrusiveLinked * _linked )
			{
				ConstStringHolder * elem = static_cast<ConstStringHolder *>(_linked);

				elem->combine_other( m_out );
			}

		protected:
			ConstStringHolder * m_out;
		};
		//////////////////////////////////////////////////////////////////////////
		void ConstStringHolder::combine_from( ConstStringHolder * _from, ConstStringHolder * _out )
		{
			_from->m_owner->release_string();
			
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
		//////////////////////////////////////////////////////////////////////////			
		void ConstStringHolder::hash()
		{
			const char * str = m_value.c_str();

			m_lesshash = 5381;
			int c;

			while(c = *str++)
			{
				m_lesshash = ((m_lesshash << 5) + m_lesshash) + c; /* hash * 33 + c */
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString()
		: m_holder(none.m_holder)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString( const ConstString & _cstr )
		: m_holder(_cstr.m_holder->owner())
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString( char * _str )
		: m_holder( new Detail::ConstStringHolder(_str) )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString( const char * _str )
		: m_holder( new Detail::ConstStringHolder(_str) )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString( const char * _str, size_t _size )
		: m_holder( new Detail::ConstStringHolder(_str, _size) )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString( const std::string & _str )
		: m_holder( new Detail::ConstStringHolder(_str) )
	{
	}
#	endif
}
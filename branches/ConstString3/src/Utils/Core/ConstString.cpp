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

			m_hash = this->hash();
		}
		//////////////////////////////////////////////////////////////////////////
		bool ConstStringHolder::less( ConstStringHolder * _holder )
		{
			if( m_hash < _holder->m_hash )
			{
				return true;
			}
			else if( m_hash > _holder->m_hash )
			{
				return false;
			}
			else if( m_owner == _holder->m_owner )
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
			else if( m_hash != _holder->m_hash )
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
		class ConstStringHolder::ForeachCombineOwner
		{
		public:
			ForeachCombineOwner( ConstStringHolder * _out )
				: m_out(_out)
			{
			}

		public:
			void operator () ( IntrusiveLinked * _linked ) const
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
			ForeachCombineOther( ConstStringHolder * _out )
				: m_out(_out)
			{
			}

		public:
			void operator () ( IntrusiveLinked * _linked ) const
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
					from_owner->destroy();
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
		size_t ConstStringHolder::hash() const
		{
			const char * str = m_value.c_str();

			size_t hash = 5381;
			int c;

			while(c = *str++)
			{
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
			}

			return hash;
		}
		//////////////////////////////////////////////////////////////////////////
		ConstStringMemory::ConstStringMemory( const std::string & _value )
			: ConstStringHolder(_value)
		{
		}
		//////////////////////////////////////////////////////////////////////////
		void ConstStringMemory::release_string()
		{
			std::string empty;
			m_value.swap(empty);
		}
		//////////////////////////////////////////////////////////////////////////
		void ConstStringMemory::destroy()
		{
			delete this;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString()
		: m_holder(none.m_holder->owner())
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString( const ConstString & _cstr )
		: m_holder(_cstr.m_holder->owner())
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString( const char * _str )
		: m_holder( new Detail::ConstStringMemory(std::string(_str)) )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString( const char * _str, size_t _size )
		: m_holder( new Detail::ConstStringMemory(std::string(_str, _size)) )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString::ConstString( const std::string & _str )
		: m_holder( new Detail::ConstStringMemory(_str) )
	{
	}
#	endif
}
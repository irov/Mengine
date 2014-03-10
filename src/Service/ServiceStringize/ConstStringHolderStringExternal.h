#   pragma once

#	include "Factory/Factorable.h"

#   include "stdex/const_string_holder.h"

namespace Menge
{
	class ConstStringHolderStringExternal
		: public stdex::const_string_holder
		, public Factorable
	{
	public:
		void setValue( const char * _value, size_t _size )
		{
			m_value = _value;

			this->setup( m_value, _size, false );
		}

	protected:
		void _releaseString() override
		{
			//Empty
		}

	protected:
		void _destroyString() override
		{
			this->destroy();
		}

	protected:			
		const char * m_value;
	};
}
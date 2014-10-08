#   pragma once

#	include "Factory/Factorable.h"

#   include "stdex/const_string_holder.h"

#	include "pybind/system.hpp"

#	include "Core/Hash.h"

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
			
			hash_type hash = Helper::makeHash( _value, _size );

			this->setup( m_value, _size, hash, false );
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
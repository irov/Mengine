#   pragma once

#	include "Factory/Factorable.h"

#   include "stdex/const_string_holder.h"

#	include "pybind/system.hpp"

namespace Menge
{
	class ConstStringHolderStringExternal
		: public stdex::const_string_holder
		, public Factorable
	{
	public:
		void setValue( const char * _value, size_t _size, hash_type _hash )
		{
			this->setup( _value, _size, _hash, false );
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
	};
}
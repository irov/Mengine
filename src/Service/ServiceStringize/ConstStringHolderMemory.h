#   pragma once

#   include "stdex/const_string_holder.h"

#	include "Factory/Factorable.h"

namespace Menge
{
    class ConstStringHolderMemory
        : public stdex::const_string_holder
        , public Factorable
    {
	public:
		ConstStringHolderMemory();

	public:
		void setValue( const char * _value, size_t _size, hash_type _hash );

    protected:
		void _releaseString() override;
		void _destroyString() override;

    protected:			
		char * m_buff;
    };
}
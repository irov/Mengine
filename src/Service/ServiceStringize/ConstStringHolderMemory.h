#   pragma once

#   include "stdex/const_string2_holder.h"

#	include "Core/ConstStringHolder.h"

namespace Menge
{
    class ConstStringHolderMemory
        : public ConstStringHolder
    {
	public:
		ConstStringHolderMemory();

	public:
		void setValue( const char * _value, size_t _size, hash_type _hash );

    protected:
		void destroyString() override;

    protected:			
		char * m_buff;
    };

    typedef stdex::intrusive_ptr<ConstStringHolderMemory> ConstStringHolderMemoryPtr;
}
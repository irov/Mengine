#include "ConstStringHolderMemory.h"

#include "Kernel/MemoryAllocator.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderMemory::ConstStringHolderMemory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderMemory::~ConstStringHolderMemory()
    {
        const value_type * value = this->data();

        Helper::deallocateMemory( const_cast<value_type *>(value), "const_string" );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderMemory::setValue( const ConstStringHolder::value_type * _value, ConstStringHolder::size_type _size, hash_type _hash )
    {
        ConstStringHolder::value_type * buff = Helper::allocateMemoryNT<ConstStringHolder::value_type>( _size + 1, "const_string" );
        stdex::memorycopy( buff, 0, _value, _size );
        buff[_size] = '\0';

        this->setup( buff, _size, _hash );
    }
    //////////////////////////////////////////////////////////////////////////
}
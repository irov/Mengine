#include "ConstStringHolderMemory.h"

#include "Kernel/MemoryAllocator.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderMemory::ConstStringHolderMemory()
        : m_buff( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderMemory::~ConstStringHolderMemory()
    {
        if( m_buff != nullptr )
        {
            Helper::freeArrayT( m_buff );
            m_buff = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderMemory::setValue( const ConstStringHolder::value_type * _value, ConstStringHolder::size_type _size, hash_type _hash )
    {
        m_buff = Helper::allocateArrayT<ConstStringHolder::value_type>( _size + 1 );
        stdex::memorycopy( m_buff, 0, _value, _size );
        m_buff[_size] = '\0';

        this->setup( m_buff, _size, _hash );
    }
}
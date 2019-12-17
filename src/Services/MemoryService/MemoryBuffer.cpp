#include "MemoryBuffer.h"

#include "Kernel/MemoryAllocator.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryBuffer::MemoryBuffer()
        : m_memory( nullptr )
        , m_size( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryBuffer::~MemoryBuffer()
    {
        Helper::freeMemory( m_memory, MENGINE_DOCUMENTABLE_STR() );

        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryBuffer::setBuffer( const void * _ptr, size_t _size )
    {
        void * buffer = this->newBuffer( _size );

        stdex::memorycopy( buffer, 0, _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryBuffer::newBuffer( size_t _size )
    {
        void * new_memory = Helper::reallocateMemory( m_memory, _size, MENGINE_DOCUMENTABLE_STR() );

        if( new_memory == nullptr )
        {
            m_memory = nullptr;
            m_size = 0;

            return nullptr;
        }

        m_memory = new_memory;
        m_size = _size;

        return m_memory;
    }
}

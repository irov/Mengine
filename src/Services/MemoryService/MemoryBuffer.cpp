#include "MemoryBuffer.h"

#include "Kernel/MemoryAllocator.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryBuffer::MemoryBuffer()
        : m_memory( nullptr )
        , m_size( 0 )
#ifdef MENGINE_DEBUG
        , m_doc( nullptr )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryBuffer::~MemoryBuffer()
    {
        Helper::freeMemory( m_memory, m_doc );
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryBuffer::setBuffer( const void * _ptr, size_t _size, const Char * _doc )
    {
        void * buffer = this->newBuffer( _size, _doc );

        stdex::memorycopy( buffer, 0, _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryBuffer::newBuffer( size_t _size, const Char * _doc )
    {
        void * new_memory = Helper::reallocateMemory( m_memory, _size, _doc );

        if( new_memory == nullptr )
        {
            m_memory = nullptr;
            m_size = 0;

            return nullptr;
        }

        m_memory = new_memory;
        m_size = _size;

#ifdef MENGINE_DEBUG
        m_doc = _doc;
#endif

        return m_memory;
    }
}

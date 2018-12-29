#include "MemoryStore.h"

#include "MemoryAllocator.h"

namespace Mengine
{
    MemoryStore::MemoryStore()
        : m_memory( nullptr )
        , m_doc( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryStore::~MemoryStore()
    {
        if( m_memory != nullptr )
        {
            Helper::freeMemory( m_memory, m_doc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryStore::allocate( size_t _size, const Char * _doc )
    {
        m_memory = Helper::allocateMemory( _size, _doc );
        m_doc = _doc;

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryStore::free()
    {
        Helper::freeMemory( m_memory, m_doc );
        m_memory = nullptr;
    }
}

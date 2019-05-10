#include "MemoryStore.h"

#include "MemoryAllocator.h"

namespace Mengine
{
    MemoryStore::MemoryStore()
        : m_memory( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryStore::~MemoryStore()
    {
        if( m_memory != nullptr )
        {
#ifdef MENGINE_DEBUG
            Helper::freeMemory( m_memory, m_doc.c_str() );
#else
            Helper::freeMemory( m_memory, "MemoryStore" );
#endif
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryStore::allocate( size_t _size, const Char * _doc )
    {
#ifdef MENGINE_DEBUG
        m_memory = Helper::allocateMemory( _size, _doc );

        m_doc = _doc;
#else
        m_memory = Helper::allocateMemory( _size, "MemoryStore" );
#endif

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryStore::free()
    {
#ifdef MENGINE_DEBUG
        Helper::freeMemory( m_memory, m_doc.c_str() );
#else
        Helper::freeMemory( m_memory, "MemoryStore" );
#endif        

        m_memory = nullptr;
    }
}

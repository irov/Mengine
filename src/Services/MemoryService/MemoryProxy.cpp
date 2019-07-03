#include "MemoryProxy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryProxy::MemoryProxy()
        : m_ptr( nullptr )
        , m_size( 0 )
#ifdef MENGINE_DEBUG
        , m_doc( nullptr )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryProxy::~MemoryProxy()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryProxy::setBuffer( void * _ptr, size_t _size, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        m_ptr = _ptr;
        m_size = _size;

#ifdef MENGINE_DEBUG
        m_doc = _doc;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryProxy::getBuffer() const
    {
        return m_ptr;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MemoryProxy::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryProxy::empty() const
    {
        return false;
    }
}
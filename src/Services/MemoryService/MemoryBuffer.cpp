#include "MemoryBuffer.h"

#include "Kernel/MemoryAllocator.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryBuffer::MemoryBuffer()
        : m_memory( nullptr )
        , m_doc( nullptr )
        , m_size( 0 )
#ifdef MENGINE_DEBUG
        , m_file( nullptr )
        , m_line( 0 )
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
    void MemoryBuffer::setBuffer( const void * _ptr, size_t _size, const Char * _doc, const Char * _file, uint32_t _line )
    {
        void * buffer = this->newBuffer( _size, _doc, _file, _line );

        stdex::memorycopy( buffer, 0, _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryBuffer::newBuffer( size_t _size, const Char * _doc, const Char * _file, uint32_t _line )
    {
        MENGINE_UNUSED( _file );
        MENGINE_UNUSED( _line );

        void * new_memory = Helper::reallocateMemory( m_memory, _size, _doc );

        if( new_memory == nullptr )
        {
            m_memory = nullptr;
            m_size = 0;

            return nullptr;
        }

        m_memory = new_memory;
        m_doc = _doc;
        m_size = _size;

#ifdef MENGINE_DEBUG
        m_file = _file;
        m_line = _line;
#endif

        return m_memory;
    }
}

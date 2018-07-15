#include "MemoryBuffer.h"

#include "Kernel/MemoryAllocator.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryBuffer::MemoryBuffer()
        : m_memory( nullptr )
        , m_size( 0 )
#ifndef NDEBUG
        , m_file( nullptr )
        , m_line( 0 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryBuffer::~MemoryBuffer()
    {
        Helper::freeMemory( m_memory, "MemoryBuffer" );
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryBuffer::setMemory( const void * _ptr, size_t _size, const Char * _file, uint32_t _line )
    {
        void * buffer = this->newMemory( _size, _file, _line );

        stdex::memorycopy( buffer, 0, _ptr, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MemoryBuffer::newMemory( size_t _size, const char * _file, uint32_t _line )
    {
        (void)_file;
        (void)_line;

        void * new_memory = Helper::reallocateMemory( m_memory, _size, "MemoryBuffer" );

        if( new_memory == nullptr )
        {
            m_memory = nullptr;
            m_size = 0;

            return nullptr;
        }

        m_memory = new_memory;
        m_size = _size;

#ifndef NDEBUG
        m_file = _file;
        m_line = _line;
#endif

        return m_memory;
    }
}

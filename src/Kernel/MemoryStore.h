#pragma once

#include "Kernel/Pointer.h"
#include "Config/String.h"

namespace Mengine
{
    class MemoryStore
    {
    public:
        MemoryStore();
        ~MemoryStore();

    public:
        Pointer allocate( size_t _size, const Char * _doc );
        void free();

    protected:
        void * m_memory;

#ifdef MENGINE_DEBUG
        String m_doc;
#endif

    private:
        MemoryStore( const MemoryStore & );
        MemoryStore & operator = ( const MemoryStore & );
    };
}
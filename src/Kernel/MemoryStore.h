#pragma once

#include "Kernel/Pointer.h"
#include "Config/Char.h"

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
        const Char * m_doc;

    private:
        MemoryStore( const MemoryStore & );
        MemoryStore & operator = ( const MemoryStore & );
    };
}
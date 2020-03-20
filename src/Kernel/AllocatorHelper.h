#pragma once

#include "Config/Config.h"
#include "Config/Char.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        void * allocateMemory( size_t _size, const Char * _doc );
        void * callocateMemory( size_t _num, size_t _size, const Char * _doc );
        void * reallocateMemory( void * _buffer, size_t _size, const Char * _doc );
        void deallocateMemory( void * _memory, const Char * _doc );
    }
}
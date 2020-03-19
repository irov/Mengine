#pragma once

#include "Config/Config.h"
#include "Config/Char.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        void * allocateMemory( size_t _size, const Char * _doc );
        void * reallocateMemory( void * _buffer, size_t _size, const Char * _doc );
        void freeMemory( void * _memory, const Char * _doc );
    }
}
#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        bool Win32GetLocalMachineRegValue( const WChar * _path, const WChar * _key, Char * const _value, size_t _size );
    }
}
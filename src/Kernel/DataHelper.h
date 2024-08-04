#pragma once

#include "Kernel/Data.h"
#include "Kernel/String.h"

namespace Mengine
{
    namespace Helper
    {        
        //////////////////////////////////////////////////////////////////////////
        void stringFromData( const Data & _data, String * const _str );
        void dataFromString( const String & _str, Data * const _data );
        //////////////////////////////////////////////////////////////////////////
    }
}

#pragma once

#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void memoryZero( void * _dst, size_t _size );
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void memoryZeroPod( T * _obj )
        {
            const size_t sizeof_T = sizeof( T );

            Helper::memoryZero( _obj, sizeof_T );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
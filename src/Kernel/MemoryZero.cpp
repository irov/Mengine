#include "MemoryZero.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void memoryZero( void * _dst, size_t _size )
        {
            StdString::memset( _dst, 0x00, _size );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
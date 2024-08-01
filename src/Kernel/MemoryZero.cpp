#include "MemoryZero.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void memoryZero( void * _dst, size_t _size )
        {
            MENGINE_MEMSET( _dst, 0x00, _size );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
#include "BufferHelper.h"

#include "Kernel/Assertion.h"

#include "Config/StdIO.h"
#include "Config/StdArg.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        size_t bufferFormat( Char * const _buffer, size_t _capacity, const Char * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            int32_t size_vsnprintf = MENGINE_VSNPRINTF( _buffer, _capacity, _format, args );
            
            MENGINE_VA_LIST_END( args );

            MENGINE_ASSERTION_FATAL( size_vsnprintf >= 0, "invalid format '%s'"
                , _format
            );

            return (size_t)size_vsnprintf;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
#include "StringCopy.h"

#include "Kernel/Assertion.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        size_t stringCopy( Char * _out, const Char * _in, size_t _capacity )
        {
            size_t in_size = MENGINE_STRLEN( _in );

            MENGINE_ASSERTION_FATAL( in_size < _capacity );

            if( in_size >= _capacity )
            {
                in_size = _capacity - 1;
            }

            MENGINE_STRNCPY( _out, _in, in_size );
            _out[in_size] = '\0';

            return in_size;
        }
    }
}

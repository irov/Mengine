#include "POSIXFileHelper.h"

#include <unistd.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool POSIXGetCurrentDirectory( Char * const _directory, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                return false;
            }

            _directory[0] = '\0';

            if( ::getcwd( _directory, _capacity ) == nullptr )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}

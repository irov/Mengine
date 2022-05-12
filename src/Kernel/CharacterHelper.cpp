#include "CharacterHelper.h"

namespace Mengine
{
    namespace Helper
    {
        bool isStandardCharacterSet( Char _ch )
        {
            if( _ch < 32 || _ch > 127 )
            {
                return false;
            }

            return true;
        }
    }
}
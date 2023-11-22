#include "CharacterHelper.h"

#include "Config/StdCType.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool isASCIICharacterSet( Char _ch )
        {
            if( _ch < 0 || _ch > 127 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isStandardCharacterSet( Char _ch )
        {
            if( _ch < 32 || _ch > 127 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isLowerCharacterSet( Char _ch )
        {
            bool result = MENGINE_ISLOWER( _ch );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isUpperCharacterSet( Char _ch )
        {
            bool result = MENGINE_ISUPPER( _ch );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isDigitCharacterSet( Char _ch )
        {
            bool result = MENGINE_ISDIGIT( _ch );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
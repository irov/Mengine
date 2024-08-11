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
            bool result = StdCType::islower( _ch );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isUpperCharacterSet( Char _ch )
        {
            bool result = StdCType::isupper( _ch );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool isDigitCharacterSet( Char _ch )
        {
            bool result = StdCType::isdigit( _ch );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
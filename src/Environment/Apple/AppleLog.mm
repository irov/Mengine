#include "AppleLog.h"

#include "Kernel/StringCopy.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        /////////////////////////////////////////////////////////////////////////
        void AppleLog( NSString * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );
            
            Helper::AppleLogV( _format, args );
            
            MENGINE_VA_LIST_END( args );
        }
        /////////////////////////////////////////////////////////////////////////
        void AppleLogV( NSString * _format, MENGINE_VA_LIST_TYPE _args )
        {
            NSLogv( _format, _args );
        }
        /////////////////////////////////////////////////////////////////////////
    }
}

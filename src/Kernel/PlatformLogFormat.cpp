#include "PlatformLogFormat.h"

#include "Config/StdIO.h"
#include "Config/StdArg.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void PlatformLogMessage( const Char * _message )
        {
            std::printf( "%s", _message );
        }
        //////////////////////////////////////////////////////////////////////////
        void PlatformLogFormat( const Char * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );
            std::vprintf( _format, args );
            MENGINE_VA_LIST_END( args );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}

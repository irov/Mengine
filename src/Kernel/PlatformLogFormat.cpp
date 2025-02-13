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
            std::fprintf( stdout, "%s", _message );
        }
        //////////////////////////////////////////////////////////////////////////
        void PlatformLogFormat( const Char * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );
            std::vfprintf( stdout, _format, args );
            MENGINE_VA_LIST_END( args );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}

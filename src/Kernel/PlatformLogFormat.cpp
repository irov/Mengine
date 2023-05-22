#include "PlatformLogFormat.h"

#if defined(MENGINE_PLATFORM_APPLE)
#   include "Environment/Apple/AppleUtils.h"
#elif defined(MENGINE_PLATFORM_ANDROID)
#   include <android/log.h>
#else
#   include "Config/StdIO.h"
#endif

#   include "Config/StdArg.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void PlatformLogMessage( const Char * _message )
        {
#if defined(MENGINE_PLATFORM_APPLE)
            Helper::AppleLogMessage( _message );
#elif defined(MENGINE_PLATFORM_ANDROID)
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "%s", _message );
#else
            ::printf( "%s", _message );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        void PlatformLogFormat( const Char * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );            
#if defined(MENGINE_PLATFORM_APPLE)
            Helper::AppleLogFormatV( _format, args );
#elif defined(MENGINE_PLATFORM_ANDROID)
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", _format, args );
#else
            ::vprintf( _format, args );
#endif
            MENGINE_VA_LIST_END( args );
        }
    }
}

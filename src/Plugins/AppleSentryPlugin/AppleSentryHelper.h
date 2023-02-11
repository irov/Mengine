#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        void appleSentryCapture( const Char * _msg, int32_t _code );

        void appleSentrySetExtraBoolean( const Char * _key, int32_t _value );
        void appleSentrySetExtraInteger( const Char * _key, int32_t _value );
        void appleSentrySetExtraString( const Char * _key, const Char * _value );
    }
}

#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    namespace Helper
    {
        void appleSentryCapture( const Char * _msg, int32_t _code );

        void appleSentrySetExtraBoolean( const Char * _key, int32_t _value );
        void appleSentrySetExtraInteger( const Char * _key, int32_t _value );
        void appleSentrySetExtraString( const Char * _key, const Char * _value );
        void appleSentrySetExtraNSString( const Char * _key, NSString * _value );
    }
}

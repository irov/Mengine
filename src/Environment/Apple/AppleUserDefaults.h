#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        bool AppleGetUserDefaultsString( const Char * _key, Char * const _value, size_t _capacity );
        bool AppleSetUserDefaultsString( const Char * _key, const Char * _value );
        bool AppleGetUserDefaultsBoolean( const Char * _key, bool _default );
        bool AppleSetUserDefaultsBoolean( const Char * _key, bool _value );

        bool AppleRemoveUserDefaults( const Char * _key );
    }
}

#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"
 
namespace Mengine
{
    namespace Helper
    {
        bool AppleOpenUrlInDefaultBrowser( const Char * _url );
        bool AppleGetUserDefaultsString( const Char * _key, Char * const _value, size_t _capacity );
        bool AppleSetUserDefaultsString( const Char * _key, const Char * _value );
        bool AppleRemoveUserDefaultsString( const Char * _key );
    }
}

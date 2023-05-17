#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"
 
namespace Mengine
{
    namespace Helper
    {
        void AppleLogMessage( const Char * _message );
        void AppleLogFormat( const Char * _format, ... );
        void AppleLogFormatV( const Char * _format, MENGINE_VA_LIST_TYPE _va );
        bool AppleOpenUrlInDefaultBrowser( const Char * _url );
        bool AppleGetUserDefaultsString( const Char * _key, Char * const _value, size_t _capacity );
        bool AppleSetUserDefaultsString( const Char * _key, const Char * _value );
        bool AppleRemoveUserDefaultsString( const Char * _key );
    }
}

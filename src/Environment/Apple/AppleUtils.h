#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"
#include "Config/StdArg.h"

namespace Mengine
{
    namespace Helper
    {
        void AppleLogMessage( const Char * _message );
        void AppleLogFormat( const Char * _format, ... );
        void AppleLogFormatV( const Char * _format, MENGINE_VA_LIST_TYPE _va );
        bool AppleOpenUrlInDefaultBrowser( const Char * _url );
    }
}

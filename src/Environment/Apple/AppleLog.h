#pragma once

#include "Config/StdArg.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    namespace Helper
    {
        void AppleLog( NSString * _format, ... ) NS_FORMAT_FUNCTION(1, 2);
        void AppleLogV( NSString * _format, MENGINE_VA_LIST_TYPE _va );
    }
}

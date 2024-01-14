#pragma once

#include "Kernel/StaticString.h"

#import <Foundation/Foundation.h>

#ifndef MENGINE_APPLE_MESSAGE_NSERROR 
#define MENGINE_APPLE_MESSAGE_NSERROR 4096
#endif
 
namespace Mengine
{
    namespace Helper
    {
        typedef StaticString<MENGINE_APPLE_MESSAGE_NSERROR> NSErrorMessage;
        NSErrorMessage AppleGetMessageFromNSError( NSError * _error );
    }
}

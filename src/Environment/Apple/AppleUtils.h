#pragma once

#include "Config/Config.h"

#include "Kernel/StaticString.h"

#ifndef MENGINE_APPLE_MESSAGE_NSERROR 
#define MENGINE_APPLE_MESSAGE_NSERROR 4096
#endif
 
namespace Mengine
{
    typedef StaticString<MENGINE_APPLE_MESSAGE_NSERROR> NSErrorMessage;

    NSErrorMessage AppleGetMessageFromNSError( NSError * _error );
}

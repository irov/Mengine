#pragma once

#include "Config/Config.h"

#include "Kernel/StaticString.h"
#include "Kernel/Map.h"
#include "Kernel/String.h"

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
        
        void AppleGetMapNSDictionary( NSDictionary * _dictionary, Map<String, String> * const _map );
    }
}

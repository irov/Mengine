#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace Mengine
{
    namespace Helper
    {
        UIViewController * iOSGetRootViewController();
        NSUUID * iOSGetAdIdentifier();
    }
}

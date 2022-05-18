#pragma once

#include "ApplleFacebookInterface.h"

#include "Kernel/ServiceBase.h"

#import <FBSDKCoreKit.h>
#import <FBSDKLoginKit.h>
#import <FBSDKShareKit.h>

namespace Mengine
{
    class AppleFacebookService
         : public ServiceBase<AppleFacebookServiceInterface>
    {
    public:
        AppleFacebookService();
        ~AppleFacebookService();

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
        
    public:
        void login();
        
    };
}

#pragma once

#include "AppleFacebookInterface.h"

#include "Kernel/ServiceBase.h"

#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
#import <FBSDKShareKit/FBSDKShareKit.h>

#include "Kernel/ConstString.h"



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
        virtual void setProvider( const AppleFacebookProviderInterfacePtr & _provider ) override;
        virtual const AppleFacebookProviderInterfacePtr & getProvider() const override;
        
    public:
        virtual void login() override;
        virtual void logout() override;
        virtual bool isLoggedIn() const override;
        virtual const char* getAccessToken() const override;
        virtual void shareLink(const char* link,const char* picture) override;
        virtual void getProfilePictureLink() override;
        
    private:
        AppleFacebookProviderInterfacePtr m_provider;
        FBSDKLoginManager *m_loginManager;
        
        bool m_isProcessed;
        NSString *m_userID;
        NSString *m_imageUrl;
        
        
    };
}

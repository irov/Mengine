#pragma once

#include "AppleFacebookInterface.h"
#include "AppleFacebookShareDelegate.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"

#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
#import <FBSDKShareKit/FBSDKShareKit.h>
#import <FBSDKShareKit/FBSDKShareKit-Swift.h>

namespace Mengine
{
    class AppleFacebookService
        : public ServiceBase<AppleFacebookServiceInterface>
    {
    public:
        AppleFacebookService();
        ~AppleFacebookService() override;

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
        virtual const Char * getAccessToken() const override;
        virtual void shareLink( const Char * link, const Char * picture ) override;
        virtual void getProfilePictureLink() override;

    protected:
        AppleFacebookProviderInterfacePtr m_provider;
        FBSDKLoginManager * m_loginManager;

        AppleFacebookShareDelegate * m_shareDelegate;

        bool m_isProcessed;
        NSString * m_userID;
        NSString * m_imageURL;
    };
}

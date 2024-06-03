#pragma once

#include "AppleFacebookInterface.h"
#include "AppleFacebookShareDelegate.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"

#import <AuthenticationServices/AuthenticationServices.h>
#import <SafariServices/SafariServices.h>

#import <FBSDKCoreKit/FBSDKCoreKit-Swift.h>
#import <FBSDKLoginKit/FBSDKLoginKit-Swift.h>
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
        void setProvider( const AppleFacebookProviderInterfacePtr & _provider ) override;
        const AppleFacebookProviderInterfacePtr & getProvider() const override;

    public:
        bool login( const VectorConstString & _permissions ) override;
        void logout() override;
        bool isLoggedIn() const override;
        bool getAccessToken( Char * const _token, size_t _capacity ) const override;
        bool getUserId( Char * const _userId, size_t _capacity ) const override;
        void shareLink( const Char * link, const Char * picture ) override;
        void getProfilePictureLink() override;

    protected:
        AppleFacebookProviderInterfacePtr m_provider;
        FBSDKLoginManager * m_loginManager;

        AppleFacebookShareDelegate * m_shareDelegate;
    };
}

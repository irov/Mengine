#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/Interface.h"

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleFacebookProviderInterface
        : public Interface
    {
    public:
        virtual void onFacebookLoginSuccess( const char* token ) = 0;
        virtual void onFacebookLoginCancel( ) = 0;
        
        virtual void onFacebookError( const char* errorMessage ) = 0;
        
        
        virtual void onFacebookShareSuccess(const char* postId) = 0;
        virtual void onFacebookShareCancel() = 0;
        virtual void onFacebookShareError(const char* errorMessage) = 0;
        
        virtual void onFacebookProfilePictureLinkGet(const char* userId, bool success, const char* pictureURL) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleFacebookProviderInterface> AppleFacebookProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class AppleFacebookServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFacebookService" )
        
    public:
        virtual void setProvider( const AppleFacebookProviderInterfacePtr & _provider ) = 0;
        virtual const AppleFacebookProviderInterfacePtr & getProvider() const = 0;
        
    public:
        virtual void login() = 0;
        virtual void logout() = 0;
        virtual bool isLoggedIn() const = 0 ;
        virtual const char* getAccessToken() const = 0 ;
        virtual void shareLink(const char* link,const char* picture) = 0;
        virtual void getProfilePictureLink() = 0;
        
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FACEBOOK_SERVICE()\
    ((Mengine::AppleFacebookServiceInterface *)SERVICE_GET(Mengine::AppleFacebookServiceInterface))
//////////////////////////////////////////////////////////////////////////

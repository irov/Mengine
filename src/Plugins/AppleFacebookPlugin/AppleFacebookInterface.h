#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/VectorConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleFacebookProviderInterface
        : public Interface
    {
    public:
        virtual void onFacebookLoginSuccess( const Char * _token ) = 0;
        virtual void onFacebookLoginCancel() = 0;

        virtual void onFacebookError( int32_t _code, const Char * _errorMessage ) = 0;

        virtual void onFacebookShareSuccess( const Char * _postId ) = 0;
        virtual void onFacebookShareCancel() = 0;
        virtual void onFacebookShareError( int32_t _code, const Char * _errorMessage ) = 0;

        virtual void onFacebookProfilePictureLinkGet( const Char * _userId, bool _success, const Char * _pictureURL ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleFacebookProviderInterface> AppleFacebookProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AppleFacebookServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleFacebookService" )

    public:
        virtual void setProvider( const AppleFacebookProviderInterfacePtr & _provider ) = 0;
        virtual const AppleFacebookProviderInterfacePtr & getProvider() const = 0;

    public:
        virtual void login( const VectorConstString & _permissions ) = 0;
        virtual void logout() = 0;
        virtual bool isLoggedIn() const = 0;
        virtual const Char * getAccessToken() const = 0;
        virtual void shareLink( const Char * link, const Char * picture ) = 0;
        virtual void getProfilePictureLink() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FACEBOOK_SERVICE()\
    ((Mengine::AppleFacebookServiceInterface *)SERVICE_GET(Mengine::AppleFacebookServiceInterface))
//////////////////////////////////////////////////////////////////////////

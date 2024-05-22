#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/Params.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleFacebookProviderInterface
        : public Interface
    {
    public:
        virtual void onFacebookLoginSuccess( const Params & _params ) = 0;
        virtual void onFacebookLoginCancel() = 0;

        virtual void onFacebookError( int32_t _code, const Char * _errorMessage ) = 0;

        virtual void onFacebookShareSuccess( const Char * _postId ) = 0;
        virtual void onFacebookShareCancel() = 0;
        virtual void onFacebookShareError( int32_t _code, const Char * _errorMessage ) = 0;

        virtual void onFacebookProfilePictureLinkGetSuccess( const Char * _userId, bool _success, const Char * _pictureURL ) = 0;
        virtual void onFacebookProfilePictureLinkGetError( int32_t _code, const Char * _errorMessage ) = 0;
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
        virtual bool login( bool _limited, const VectorConstString & _permissions ) = 0;
        virtual void logout() = 0;
        virtual bool isAccessSuccess() const = 0;
        virtual bool isAuthenticationSuccess() const = 0;
        virtual bool getAccessToken( Char * const _token, size_t _capacity ) const = 0;
        virtual bool getAuthenticationToken( Char * const _token, size_t _capacity ) const = 0;
        virtual bool getUserId( Char * const _userId, size_t _capacity ) const = 0;
        virtual void shareLink( const Char * link, const Char * picture ) = 0;
        virtual void getProfilePictureLink() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_FACEBOOK_SERVICE()\
    ((Mengine::AppleFacebookServiceInterface *)SERVICE_GET(Mengine::AppleFacebookServiceInterface))
//////////////////////////////////////////////////////////////////////////

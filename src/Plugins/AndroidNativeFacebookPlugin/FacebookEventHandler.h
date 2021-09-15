#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EnumFacebookEventHandler
    {
        FACEBOOK_INITIALIZE,
        FACEBOOK_LOGIN_SUCCESS,
        FACEBOOK_LOGIN_CANCEL,
        FACEBOOK_LOGIN_ERROR,
        FACEBOOK_LOGOUT_SUCCESS,
        FACEBOOK_LOGOUT_CANCEL,
        FACEBOOK_LOGOUT_ERROR,
        FACEBOOK_SHARE_SUCCESS,
        FACEBOOK_SHARE_CANCEL,
        FACEBOOK_SHARE_ERROR,
        FACEBOOK_USER_FETCH_SUCCESS,
        FACEBOOK_PROFILE_PICTURE_LINK_GET,
    };
    //////////////////////////////////////////////////////////////////////////
    class FacebookEventHandler
        : public Mixin
    {
    public:
        virtual void onFacebookInitialized() = 0;

    public:
        virtual void onFacebookLoginSuccess( const String & _token ) = 0;
        virtual void onFacebookLoginCancel() = 0;
        virtual void onFacebookLoginError( const String & _exception ) = 0;

    public:
        virtual void onFacebookLogoutSuccess() = 0;
        virtual void onFacebookLogoutCancel() = 0;
        virtual void onFacebookLogoutError( const String & _exception ) = 0;

    public:
        virtual void onFacebookShareSuccess( const String & _postId ) = 0;
        virtual void onFacebookShareCancel() = 0;
        virtual void onFacebookShareError( const String & _exception ) = 0;

    public:
        virtual void onFacebookUserFetchSuccess( const String & _userObject, const String & _response ) = 0;

    public:
        virtual void onProfilePictureLinkGet( const String & _pictureURL ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FacebookEventHandler> FacebookEventHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
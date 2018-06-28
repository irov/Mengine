#pragma once

#include "Core/Mixin.h"
#include "Core/IntrusivePtr.h"

#include "Config/String.h"

namespace Mengine
{
    class FacebookLoginCallback
        : public Mixin
    {
    public:
        virtual void onLoginSuccess( const String & _token ) = 0;
        virtual void onLoginCancel() = 0;
        virtual void onLoginError( const String & _exception ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FacebookLoginCallback> FacebookLoginCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}
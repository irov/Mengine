#pragma once

#include "Core/Mixin.h"
#include "Core/IntrusivePtr.h"

#include "Config/String.h"

namespace Mengine
{
    class FacebookUserCallback 
        : public Mixin
    {
    public:
        virtual void onUserFetchSuccess( const String & _userObject, const String & _response ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FacebookUserCallback> FacebookUserCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}
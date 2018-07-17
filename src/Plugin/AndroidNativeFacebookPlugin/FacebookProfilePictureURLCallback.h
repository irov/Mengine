#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

#include "Config/String.h"

namespace Mengine
{
    class FacebookProfilePictureURLCallback
        : public Mixin
    {
    public:
        virtual void onProfilePictureLinkGet( const String & _pictureURL ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FacebookProfilePictureURLCallback> FacebookProfilePictureURLCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}

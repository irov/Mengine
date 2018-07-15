#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

#include "Config/String.h"

namespace Mengine
{
    class FacebookShareCallback 
        : public Mixin
    {
    public:
        virtual void onShareSuccess( const String & _postId ) = 0;
        virtual void onShareCancel() = 0;
        virtual void onShareError( const String & _exception ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FacebookShareCallback> FacebookShareCallbackPtr;
    //////////////////////////////////////////////////////////////////////////
}
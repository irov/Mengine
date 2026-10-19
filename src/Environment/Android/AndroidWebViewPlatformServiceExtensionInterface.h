#pragma once

#include "Interface/WebViewPlatformExtensionInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidWebViewEvent.h"

namespace Mengine
{
    class AndroidWebViewPlatformServiceExtensionInterface
        : public WebViewPlatformExtensionInterface
    {
    public:
        virtual void processEvent( JNIEnv * _jenv, jclass _jclass, EAndroidWebViewEvent _id, jobject _event ) = 0;
        virtual void postCommand( const jpp::object & _command ) const = 0;
    };
}

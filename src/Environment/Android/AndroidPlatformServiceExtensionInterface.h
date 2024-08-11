#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/Android/AndroidIncluder.h"

namespace Mengine
{
    class AndroidPlatformServiceExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual void setAndroidNativeWindow( ANativeWindow * _nativeWindow ) = 0;
        virtual void destroyAndroidNativeWindow( ANativeWindow * _nativeWindow ) = 0;
        virtual void changeAndroidNativeWindow( ANativeWindow * _nativeWindow, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate ) = 0;
    };
}
#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/Android/AndroidIncluder.h"

namespace Mengine
{
    class AndroidPlatformServiceExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual void androidNativeSurfaceCreated( ANativeWindow * _nativeWindow ) = 0;
        virtual void androidNativeSurfaceDestroyed( ANativeWindow * _nativeWindow ) = 0;
        virtual void androidNativeSurfaceChanged( ANativeWindow * _nativeWindow, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate ) = 0;

    public:
        virtual void androidNativeKeyEvent( jboolean _isDown, jint _keyCode, jint _repeatCount ) = 0;
        virtual void androidNativeTextEvent( jint unicode ) = 0;
        virtual void androidNativeTouchEvent( jint _action, jint _pointerId, jfloat _x, jfloat _y, jfloat _pressure ) = 0;
        virtual void androidNativeAccelerationEvent( jfloat _x, jfloat _y, jfloat _z ) = 0;
        virtual void androidNativePauseEvent() = 0;
        virtual void androidNativeResumeEvent() = 0;
    };
}
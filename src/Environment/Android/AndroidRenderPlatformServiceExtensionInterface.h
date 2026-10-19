#pragma once

#include "Environment/Android/AndroidPlatformServiceExtensionInterface.h"

namespace Mengine
{
    class AndroidRenderPlatformServiceExtensionInterface
        : public AndroidPlatformServiceExtensionInterface
    {
    public:
        virtual jfloat androidNativeGetLastFingerX() const = 0;
        virtual jfloat androidNativeGetLastFingerY() const = 0;
        virtual jfloat androidNativeGetLastFingerPressure() const = 0;

    public:
        virtual void androidNativeSurfaceCreatedEvent( ANativeWindow * _nativeWindow ) = 0;
        virtual void androidNativeSurfaceDestroyedEvent() = 0;
        virtual void androidNativeSurfaceChangedEvent( ANativeWindow * _nativeWindow, jint _surfaceWidth, jint _surfaceHeight, jint _deviceWidth, jint _deviceHeight, jfloat _rate ) = 0;

    public:
        virtual void androidNativeKeyEvent( jlong _eventTime, jboolean _isDown, jint _keyCode, jint _repeatCount ) = 0;
        virtual void androidNativeControllerConnectEvent( jlong _eventTime, jint _deviceId, jboolean _connected ) = 0;
        virtual void androidNativeControllerButtonEvent( jlong _eventTime, jint _deviceId, jint _keyCode, jfloat _value, jboolean _isDown ) = 0;
        virtual void androidNativeControllerAxisEvent( jlong _eventTime, jint _deviceId, jint _axis, jfloat _value ) = 0;
        virtual void androidNativeTextEvent( jlong _eventTime, jint _unicode ) = 0;
        virtual void androidNativeTouchEvent( jlong _eventTime, jint _action, jint _pointerId, jfloat _x, jfloat _y, jfloat _pressure ) = 0;
        virtual void androidNativeAccelerationEvent( jlong _eventTime, jfloat _x, jfloat _y, jfloat _z ) = 0;
    };
}

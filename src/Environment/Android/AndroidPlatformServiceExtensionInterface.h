#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/Android/AndroidIncluder.h"

namespace Mengine
{
    class AndroidPlatformServiceExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual size_t androidNativeGetAndroidId( Char * _androidId, size_t _capacity ) const = 0;
        virtual jfloat androidNativeGetLastFingerX() const = 0;
        virtual jfloat androidNativeGetLastFingerY() const = 0;
        virtual jfloat androidNativeGetLastFingerPressure() const = 0;

    public:
        virtual void androidNativeSurfaceCreatedEvent( ANativeWindow * _nativeWindow ) = 0;
        virtual void androidNativeSurfaceDestroyedEvent() = 0;
        virtual void androidNativeSurfaceChangedEvent( ANativeWindow * _nativeWindow, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate ) = 0;

    public:
        virtual void androidNativeKeyEvent( jboolean _isDown, jint _keyCode, jint _repeatCount ) = 0;
        virtual void androidNativeTextEvent( jint unicode ) = 0;
        virtual void androidNativeTouchEvent( jint _action, jint _pointerId, jfloat _x, jfloat _y, jfloat _pressure ) = 0;
        virtual void androidNativeAccelerationEvent( jfloat _x, jfloat _y, jfloat _z ) = 0;
        virtual void androidNativePauseEvent( jfloat _x, jfloat _y ) = 0;
        virtual void androidNativeResumeEvent( jfloat _x, jfloat _y ) = 0;
        virtual void androidNativeStopEvent() = 0;
        virtual void androidNativeStartEvent() = 0;
        virtual void androidNativeRestartEvent() = 0;
        virtual void androidNativeDestroyEvent() = 0;
        virtual void androidNativeFreezeEvent( const ConstString & _owner, bool _freeze ) = 0;
        virtual void androidNativeClipboardChangedEvent() = 0;
        virtual void androidNativeWindowFocusChangedEvent( jboolean _focus ) = 0;

    public:
        virtual void lockActivity() = 0;
        virtual void unlockActivity() = 0;

    public:
        virtual void androidNativeQuitEvent() = 0;
        virtual void androidNativeLowMemoryEvent() = 0;
        virtual void androidNativeTrimMemoryEvent( jint _level ) = 0;
        virtual void androidNativeChangeLocale( const Mengine::Char * _language ) = 0;
        virtual jboolean androidNativeProcessEvents() = 0;
    };
}
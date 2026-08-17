#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/Params.h"
#include "Kernel/String.h"

#include "Config/Lambda.h"

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
        virtual void androidNativeKeyEvent( jlong _eventTime, jboolean _isDown, jint _keyCode, jint _repeatCount ) = 0;
        virtual void androidNativeControllerConnectEvent( jlong _eventTime, jint _deviceId, jboolean _connected ) = 0;
        virtual void androidNativeControllerButtonEvent( jlong _eventTime, jint _deviceId, jint _keyCode, jfloat _value, jboolean _isDown ) = 0;
        virtual void androidNativeControllerAxisEvent( jlong _eventTime, jint _deviceId, jint _axis, jfloat _value ) = 0;
        virtual void androidNativeSafeAreaViewportEvent( jfloat _beginX, jfloat _beginY, jfloat _endX, jfloat _endY ) = 0;
        virtual void androidNativeTextEvent( jlong _eventTime, jint unicode ) = 0;
        virtual void androidNativeTouchEvent( jlong _eventTime, jint _action, jint _pointerId, jfloat _x, jfloat _y, jfloat _pressure ) = 0;
        virtual void androidNativeAccelerationEvent( jlong _eventTime, jfloat _x, jfloat _y, jfloat _z ) = 0;
        virtual void androidNativeIntentStartEvent( const String & _action, const String & _data, const String & _type, uint32_t _flags, const Params & _extras ) = 0;
        virtual void androidNativeIntentNewEvent( const String & _action, const String & _data, const String & _type, uint32_t _flags, const Params & _extras ) = 0;
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
        typedef Lambda<void()> LambdaNativeRunnable;
        virtual jobject createNativeRunnable( JNIEnv * _jenv, const LambdaNativeRunnable & _callback ) = 0;
        virtual void androidNativeInvokeRunnable( JNIEnv * _jenv, jobject _buffer ) = 0;
        virtual void androidNativeReleaseRunnable( JNIEnv * _jenv, jobject _buffer ) = 0;

    public:
        virtual void lockActivity() = 0;
        virtual void unlockActivity() = 0;

    public:
        virtual void androidNativeQuitEvent() = 0;
        virtual void androidNativeLowMemoryEvent() = 0;
        virtual void androidNativeTrimMemoryEvent( jint _level ) = 0;
        virtual void androidNativeChangeLocale( const Mengine::Char * _language ) = 0;
    };
}

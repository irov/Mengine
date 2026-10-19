#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/Params.h"
#include "Kernel/String.h"
#include "Kernel/Viewport.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class AndroidPlatformServiceExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual size_t androidNativeGetAndroidId( Char * _androidId, size_t _capacity ) const = 0;

    public:
        virtual void androidNativeWindowInsetsEvent( const Viewport & _safeAreaViewport, bool _displayCutoutValid, const Viewport & _displayCutoutViewport ) = 0;
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
        virtual void androidNativeDeleteAccountAccepted() = 0;
        virtual void androidNativeDeleteAccountCanceled() = 0;

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

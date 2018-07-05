#include "AndroidNativeUnityPlugin.h"

#include "Core/Callback.h"

#include "pybind/pybind.hpp"

#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <vector>
#include <pthread.h>

extern "C" {
#define UNITY_JAVA_PREFIX                                org_Mengine_Build_UnityAds
#define MENGINE_JAVA_PREFIX                              org_Mengine_Build
#define CONCAT1(prefix, class, function)                 CONCAT2(prefix, class, function)
#define CONCAT2(prefix, class, function)                 Java_ ## prefix ## _ ## class ## _ ## function
#define MENGINE_JAVA_INTERFACE(function)                 CONCAT1(MENGINE_JAVA_PREFIX, MengineActivity, function)
#define UNITY_JAVA_INTERFACE(function)                   CONCAT1(UNITY_JAVA_PREFIX, UnityAdsInteractionLayer, function)

JNIEXPORT void JNICALL
MENGINE_JAVA_INTERFACE( AndroidNativeFacebook_1setupUnityJNI )(JNIEnv *env, jclass cls);

JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsClick )(JNIEnv *env, jclass cls, jstring s_);

JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsPlacementStateChanged )(JNIEnv *env,
    jclass cls, jstring s_, jint placementState_, jint placementState1_);

JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsReady )(JNIEnv *env, jclass cls, jstring s_);

JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsStart )(JNIEnv *env, jclass cls, jstring s_);

JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsFinish )(JNIEnv *env, jclass cls, jstring s_,
    jint finishState_);

JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsError )(JNIEnv *env, jclass cls,
    jint unityAdsError_, jstring s_);

}


static pthread_key_t mThreadKey;
static JavaVM *mJavaVM;

static jclass mActivityClass;

static jmethodID jmethodID_setupAds;
static jmethodID jmethodID_showAd;

static Mengine::UnitySetupCallbackPtr g_currentUnitySetupCallback;
static Mengine::UnityShowAdCallbackPtr g_currentUnityShowAdCallback;
//////////////////////////////////////////////////////////////////////////
static void UN_JNI_ThreadDestroyed( void *value )
{
    /* The thread is being destroyed, detach it from the Java VM and set the mThreadKey value to NULL as required */
    JNIEnv *env = (JNIEnv *)value;
    if( env != NULL ) {
        mJavaVM->DetachCurrentThread();
        pthread_setspecific( mThreadKey, NULL );
    }
}
//////////////////////////////////////////////////////////////////////////
JNIEnv *UN_JNI_GetEnv( void )
{
    /* From http://developer.android.com/guide/practices/jni.html
     * All threads are Linux threads, scheduled by the kernel.
     * They're usually started from managed code (using Thread.start), but they can also be created elsewhere and then
     * attached to the JavaVM. For example, a thread started with pthread_create can be attached with the
     * JNI AttachCurrentThread or AttachCurrentThreadAsDaemon functions. Until a thread is attached, it has no JNIEnv,
     * and cannot make JNI calls.
     * Attaching a natively-created thread causes a java.lang.Thread object to be constructed and added to the "main"
     * ThreadGroup, making it visible to the debugger. Calling AttachCurrentThread on an already-attached thread
     * is a no-op.
     * Note: You can call this function any number of times for the same thread, there's no harm in it
     */

    JNIEnv *env;
    int status = mJavaVM->AttachCurrentThread( &env, NULL );
    if( status < 0 ) {
        return 0;
    }

    /* From http://developer.android.com/guide/practices/jni.html
     * Threads attached through JNI must call DetachCurrentThread before they exit. If coding this directly is awkward,
     * in Android 2.0 (Eclair) and higher you can use pthread_key_create to define a destructor function that will be
     * called before the thread exits, and call DetachCurrentThread from there. (Use that key with pthread_setspecific
     * to store the JNIEnv in thread-local-storage; that way it'll be passed into your destructor as the argument.)
     * Note: The destructor is not called unless the stored value is != NULL
     * Note: You can call this function any number of times for the same thread, there's no harm in it
     *       (except for some lost CPU cycles)
     */
    pthread_setspecific( mThreadKey, (void *)env );

    return env;
}
//////////////////////////////////////////////////////////////////////////
int UN_JNI_SetupThread( void )
{
    UN_JNI_GetEnv();

    return 1;
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM *vm, void *reserved ) {
    JNIEnv *env;
    mJavaVM = vm;
    if( mJavaVM->GetEnv( (void **)&env, JNI_VERSION_1_4 ) != JNI_OK ) {
        return -1;
    }
    /*
     * Create mThreadKey so we can keep track of the JNIEnv assigned to each thread
     * Refer to http://developer.android.com/guide/practices/design/jni.html for the rationale behind this
     */
    if( pthread_key_create( &mThreadKey, UN_JNI_ThreadDestroyed ) != 0 ) {
        return -1;
    }
    UN_JNI_SetupThread();

    return JNI_VERSION_1_4;
}

extern "C" {
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
MENGINE_JAVA_INTERFACE( AndroidNativeFacebook_1setupUnityJNI )(JNIEnv *env, jclass cls)
{
    mActivityClass = (jclass)(env->NewGlobalRef( cls ));

    jmethodID_setupAds = env->GetStaticMethodID( mActivityClass, "unitySetupAds", "(Z)V" );
    jmethodID_showAd = env->GetStaticMethodID( mActivityClass, "unityShowAd", "()V" );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsClick )(JNIEnv *env, jclass cls, jstring placementId_)
{
    const char * placementId = env->GetStringUTFChars( placementId_, 0 );

    Mengine::UnityShowAdCallbackPtr callback = g_currentUnityShowAdCallback;
    g_currentUnityShowAdCallback = nullptr;
    
    if( callback != nullptr )
    {
        callback->onUnityAdsClick( placementId );
    }
    
    env->ReleaseStringUTFChars( placementId_, placementId );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsPlacementStateChanged )(JNIEnv *env,
    jclass cls, jstring placementId_, jint placementState_, jint placementState1_)
{
    const char * placementId = env->GetStringUTFChars( placementId_, 0 );
    int placementState = static_cast<int>(placementState_);
    int placementState1 = static_cast<int>(placementState1_);
    
    Mengine::UnityShowAdCallbackPtr callback = g_currentUnityShowAdCallback;
    g_currentUnityShowAdCallback = nullptr;
    
    if( callback != nullptr )
    {
        callback->onUnityAdsPlacementStateChanged( placementId, placementState, placementState1 );
    }

    env->ReleaseStringUTFChars( placementId_, placementId );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsReady )(JNIEnv *env, jclass cls, jstring placementId_)
{
    const char * placementId = env->GetStringUTFChars( placementId_, 0 );
    
    Mengine::UnitySetupCallbackPtr callback = g_currentUnitySetupCallback;
    g_currentUnitySetupCallback = nullptr;
    
    if( callback != nullptr )
    {
        callback->onUnityAdsReady( placementId );
    }
    
    env->ReleaseStringUTFChars( placementId_, placementId );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsStart )(JNIEnv *env, jclass cls, jstring placementId_)
{
    const char * placementId = env->GetStringUTFChars( placementId_, 0 );

    Mengine::UnityShowAdCallbackPtr callback = g_currentUnityShowAdCallback;
    g_currentUnityShowAdCallback = nullptr;
    
    if( callback != nullptr )
    {
        callback->onUnityAdsStart( placementId );
    }
    
    env->ReleaseStringUTFChars( placementId_, placementId );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsFinish )(JNIEnv *env, jclass cls, jstring placementId_,
    jint finishState_)
{
    const char * placementId = env->GetStringUTFChars( placementId_, 0 );
    int finishState = static_cast<int>(finishState_);

    Mengine::UnityShowAdCallbackPtr callback = g_currentUnityShowAdCallback;
    g_currentUnityShowAdCallback = nullptr;
    
    if( callback != nullptr )
    {
        callback->onUnityAdsFinish( placementId, finishState );
    }
    
    env->ReleaseStringUTFChars( placementId_, placementId );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsError )(JNIEnv *env, jclass cls,
    jint unityAdsError_, jstring message_)
{
    int unityAdsError = static_cast<int>(unityAdsError_);
    const char * message = env->GetStringUTFChars( message_, 0 );
    
    Mengine::UnityShowAdCallbackPtr callback = g_currentUnityShowAdCallback;
    g_currentUnityShowAdCallback = nullptr;
    
    if( callback != nullptr )
    {
        callback->onUnityAdsError( unityAdsError, message );
    }

    env->ReleaseStringUTFChars( message_, message );
}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidNativeUnity, Mengine::AndroidNativeUnityPlugin )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeUnityPlugin::AndroidNativeUnityPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeUnityPlugin::~AndroidNativeUnityPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityPlugin::setupAds( const bool debug, const UnitySetupCallbackPtr & _callback )
    {
        if( g_currentUnitySetupCallback != nullptr )
        {
            return false;
        }

        g_currentUnitySetupCallback = _callback;

        JNIEnv * env = UN_JNI_GetEnv();
        
        jboolean jdebug = static_cast<jboolean>(debug);

        env->CallStaticVoidMethod( mActivityClass, jmethodID_setupAds, jdebug );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityPlugin::showAd( const UnityShowAdCallbackPtr & _callback )
    {
        if( g_currentUnityShowAdCallback != nullptr )
        {
            return false;
        }

        g_currentUnityShowAdCallback = _callback;

        JNIEnv * env = UN_JNI_GetEnv();

        env->CallStaticVoidMethod( mActivityClass, jmethodID_showAd );
        
        return true;
    }
}

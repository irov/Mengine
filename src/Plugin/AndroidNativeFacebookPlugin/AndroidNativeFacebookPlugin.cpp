#include "AndroidNativeFacebookPlugin.h"

#include "Core/Callback.h"

#include "pybind/pybind.hpp"

#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <vector>
#include <pthread.h>

extern "C" {
#define FACEBOOK_JAVA_PREFIX                             org_Mengine_Build_Facebook
#define MENGINE_JAVA_PREFIX                              org_Mengine_Build
#define CONCAT1(prefix, class, function)                 CONCAT2(prefix, class, function)
#define CONCAT2(prefix, class, function)                 Java_ ## prefix ## _ ## class ## _ ## function
#define MENGINE_JAVA_INTERFACE(function)                 CONCAT1(MENGINE_JAVA_PREFIX, MengineActivity, function)
#define FACEBOOK_JAVA_INTERFACE(function)                CONCAT1(FACEBOOK_JAVA_PREFIX, FacebookInteractionLayer, function)

JNIEXPORT void JNICALL
MENGINE_JAVA_INTERFACE( AndroidNativeFacebook_1setupFacebookJNI )(JNIEnv *mEnv, jclass cls);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginSuccess )(JNIEnv *env, jclass cls,
    jstring accessToken_);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginCancel )(JNIEnv *env, jclass cls);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginError )(JNIEnv *env, jclass cls,
    jstring exception_);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onUserFetchSuccess )(JNIEnv *env, jclass cls,
    jstring object_, jstring response_);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareSuccess )(JNIEnv *env, jclass cls,
    jstring postId_);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareCancel )(JNIEnv *env, jclass cls);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareError )(JNIEnv *env, jclass cls,
    jstring exception_);

}

static pthread_key_t mThreadKey;
static JavaVM *mJavaVM;

static jclass mActivityClass;
static jmethodID jmethodID_performLogin;
static jmethodID jmethodID_getUser;
static jmethodID jmethodID_shareLink;

static Mengine::FacebookLoginCallbackPtr g_currentFacebookLoginCallback;
static Mengine::FacebookUserCallbackPtr g_currentFacebookUserCallback;
static Mengine::FacebookShareCallbackPtr g_currentFacebookShareCallback;
//////////////////////////////////////////////////////////////////////////
static void FB_JNI_ThreadDestroyed( void *value ) 
{
    /* The thread is being destroyed, detach it from the Java VM and set the mThreadKey value to NULL as required */
    JNIEnv *env = (JNIEnv *)value;
    if( env != NULL ) {
        mJavaVM->DetachCurrentThread();
        pthread_setspecific( mThreadKey, NULL );
    }
}
//////////////////////////////////////////////////////////////////////////
JNIEnv *FB_JNI_GetEnv( void ) 
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
int FB_JNI_SetupThread( void ) 
{
    FB_JNI_GetEnv();

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
    if( pthread_key_create( &mThreadKey, FB_JNI_ThreadDestroyed ) != 0 ) {
        return -1;
    }
    FB_JNI_SetupThread();

    return JNI_VERSION_1_4;
}

extern "C" {
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
MENGINE_JAVA_INTERFACE( AndroidNativeFacebook_1setupFacebookJNI )(JNIEnv *mEnv, jclass cls)
{
    mActivityClass = (jclass)(mEnv->NewGlobalRef( cls ));

    jmethodID_performLogin = mEnv->GetStaticMethodID( mActivityClass, "facebookPerformLogin", "([Ljava/lang/String;)V" );
    jmethodID_getUser = mEnv->GetStaticMethodID( mActivityClass, "facebookGetUser", "()V" );
    jmethodID_shareLink = mEnv->GetStaticMethodID( mActivityClass, "facebookShareLink", "(Ljava/lang/String;)V" );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL 
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginSuccess )(JNIEnv *env, jclass cls, jstring accessToken_)
{
    const char * accessToken = env->GetStringUTFChars( accessToken_, 0 );

    Mengine::FacebookLoginCallbackPtr callback = g_currentFacebookLoginCallback;
    g_currentFacebookLoginCallback = nullptr;
    
    if( callback != nullptr )
    {
        callback->onLoginSuccess( accessToken );
    }

    env->ReleaseStringUTFChars( accessToken_, accessToken );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginCancel )(JNIEnv *env, jclass cls)
{
    Mengine::FacebookLoginCallbackPtr callback = g_currentFacebookLoginCallback;
    g_currentFacebookLoginCallback = nullptr;

    if( callback != nullptr )
    {
        callback->onLoginCancel();
    }
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginError )(JNIEnv *env, jclass cls, jstring exception_)
{
    const char * exception = env->GetStringUTFChars( exception_, 0 );

    Mengine::FacebookLoginCallbackPtr callback = g_currentFacebookLoginCallback;
    g_currentFacebookLoginCallback = nullptr;

    if( callback != nullptr )
    {
        callback->onLoginError( exception );
    }

    env->ReleaseStringUTFChars( exception_, exception );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onUserFetchSuccess )(JNIEnv *env, jclass cls, jstring object_, jstring response_)
{
    const char * object = env->GetStringUTFChars( object_, 0 );
    const char * response = env->GetStringUTFChars( response_, 0 );

    Mengine::FacebookUserCallbackPtr callback = g_currentFacebookUserCallback;
    g_currentFacebookUserCallback = nullptr;

    if( callback != nullptr )
    {
        callback->onUserFetchSuccess( object, response );
    }

    env->ReleaseStringUTFChars( object_, object );
    env->ReleaseStringUTFChars( response_, response );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareSuccess )(JNIEnv *env, jclass cls, jstring postId_)
{
    const char * postId = env->GetStringUTFChars( postId_, 0 );

    Mengine::FacebookShareCallbackPtr callback = g_currentFacebookShareCallback;
    g_currentFacebookShareCallback = nullptr;

    if( callback != nullptr )
    {
        callback->onShareSuccess( postId );
    }

    env->ReleaseStringUTFChars( postId_, postId );
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareCancel )(JNIEnv *env, jclass cls)
{
    Mengine::FacebookShareCallbackPtr callback = g_currentFacebookShareCallback;
    g_currentFacebookShareCallback = nullptr;

    if( callback != nullptr )
    {
        callback->onShareCancel();
    }
}
//////////////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareError )(JNIEnv *env, jclass cls, jstring exception_)
{
    const char * exception = env->GetStringUTFChars( exception_, 0 );

    Mengine::FacebookShareCallbackPtr callback = g_currentFacebookShareCallback;
    g_currentFacebookShareCallback = nullptr;

    if( callback != nullptr )
    {
        callback->onShareError( exception );
    }

    env->ReleaseStringUTFChars( exception_, exception );
}
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidNativeFacebook, Mengine::AndroidNativeFacebookPlugin )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonFacebookLoginCallback
            : public Callback<FacebookLoginCallback>
        {
        public:
            PythonFacebookLoginCallback( const pybind::object & _cb, const pybind::args & _args )
            {}

        protected:
            void onLoginSuccess( const String & _token ) override
            {
                m_cb.call_args( 0, _token, m_args );
            }

            void onLoginCancel() override
            {
                m_cb.call_args( 1, "", m_args );
            }

            void onLoginError( const String & _exception ) override
            {
                m_cb.call_args( 2, _exception, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        bool androidFacebookPerformLogin( AndroidNativeFacebookPlugin * _plugin, const TVectorString & _permissions, const pybind::object & _cb, const pybind::args & _args )
        {
            bool successful = _plugin->performLogin( _permissions
                , new FactorableUnique<PythonFacebookLoginCallback>( _cb, _args )
            );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonFacebookUserCallback
            : public Callback<FacebookUserCallback>
        {
        public:
            PythonFacebookUserCallback( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onUserFetchSuccess( const String & _userObject, const String & _response ) override
            {
                m_cb.call_args( _userObject, _response, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        bool androidFacebookGetUser( AndroidNativeFacebookPlugin * _plugin, const pybind::object & _cb, const pybind::args & _args )
        {
            bool successful = _plugin->getUser(
                new FactorableUnique<PythonFacebookUserCallback>( _cb, _args )
            );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonFacebookShareCallback
            : public Callback<FacebookShareCallback>
        {
        public:
            PythonFacebookShareCallback( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onShareSuccess( const String & _postId ) override
            {
                m_cb.call_args( 0, _postId, m_args );
            }

            void onShareCancel() override
            {
                m_cb.call_args( 1, "", m_args );
            }

            void onShareError( const String & _exception ) override
            {
                m_cb.call_args( 2, _exception, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        bool androidFacebookShareLink( AndroidNativeFacebookPlugin * _plugin, const String & _link, const pybind::object & _cb, const pybind::args & _args )
        {
            bool successful = _plugin->shareLink( _link
                , new FactorableUnique<PythonFacebookShareCallback>( _cb, _args )
            );

            return successful;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeFacebookPlugin::AndroidNativeFacebookPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeFacebookPlugin::~AndroidNativeFacebookPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookPlugin::_avaliable()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookPlugin::_initialize()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function_proxy_args( kernel, "androidFacebookPerformLogin", &Detail::androidFacebookPerformLogin, this );
        pybind::def_function_proxy_args( kernel, "androidFacebookGetUser", &Detail::androidFacebookGetUser, this );
        pybind::def_function_proxy_args( kernel, "androidFacebookShareLink", &Detail::androidFacebookShareLink, this );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeFacebookPlugin::_finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookPlugin::performLogin( const TVectorString & _permissions, const FacebookLoginCallbackPtr & _callback )
    {
        if( g_currentFacebookLoginCallback != nullptr )
        {
            return false;
        }

        g_currentFacebookLoginCallback = _callback;

        JNIEnv *mEnv = FB_JNI_GetEnv();
        
        if( _permissions.empty() == false )
        {
            TVectorString::size_type permissionsCount = _permissions.size();

            jstring jpermissions[permissionsCount];

            uint32_t jpermissionIterator = 0;
            for( const String & permission : _permissions )
            {
                const Char * permission_str = permission.c_str();

                jstring jpermission_str = mEnv->NewStringUTF( permission_str );

                jpermissions[jpermissionIterator++] = jpermission_str;
            }

            mEnv->CallStaticVoidMethod( mActivityClass, jmethodID_performLogin, jpermissions );
        }
        else
        {
            mEnv->CallStaticVoidMethod( mActivityClass, jmethodID_performLogin, nullptr );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookPlugin::getUser( const FacebookUserCallbackPtr & _callback )
    {
        if( g_currentFacebookUserCallback != nullptr )
        {
            return false;
        }

        g_currentFacebookUserCallback = _callback;

        JNIEnv *mEnv = FB_JNI_GetEnv();

        mEnv->CallStaticVoidMethod( mActivityClass, jmethodID_getUser );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookPlugin::shareLink( const String & _link, const FacebookShareCallbackPtr & _callback )
    {
        if( g_currentFacebookShareCallback != nullptr )
        {
            return false;
        }

        g_currentFacebookShareCallback = _callback;

        JNIEnv *mEnv = FB_JNI_GetEnv();

        const Char * link_str = _link.c_str();
        jstring jlink = mEnv->NewStringUTF( link_str );

        mEnv->CallStaticVoidMethod( mActivityClass, jmethodID_shareLink, jlink );
        
        return true;
    }
}

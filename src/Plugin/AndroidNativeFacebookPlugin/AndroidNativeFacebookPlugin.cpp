#include "AndroidNativeFacebookPlugin.h"

#include "Core/Callback.h"

#include "Android/AndroidUtils.h"

#include "pybind/pybind.hpp"

#define FACEBOOK_JAVA_PREFIX org_Mengine_Build_Facebook
#define FACEBOOK_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(FACEBOOK_JAVA_PREFIX, FacebookInteractionLayer, function)

extern "C" {

    JNIEXPORT void JNICALL
    MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeFacebook_1setupFacebookJNI )(JNIEnv *mEnv, jclass cls);

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

    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onProfilePictureLinkGet )(JNIEnv *env, jclass cls,
            jstring pictureURL_);
}

static jclass mActivityClass;
static jmethodID jmethodID_isLoggedIn;
static jmethodID jmethodID_performLogin;
static jmethodID jmethodID_getUser;
static jmethodID jmethodID_shareLink;
static jmethodID jmethodID_getProfilePictureLink;

static Mengine::FacebookLoginCallbackPtr g_currentFacebookLoginCallback;
static Mengine::FacebookUserCallbackPtr g_currentFacebookUserCallback;
static Mengine::FacebookShareCallbackPtr g_currentFacebookShareCallback;
static Mengine::FacebookProfilePictureURLCallbackPtr g_currentFacebookProfilePictureURLCallback;

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeFacebook_1setupFacebookJNI )(JNIEnv *mEnv, jclass cls)
    {
        mActivityClass = (jclass)(mEnv->NewGlobalRef( cls ));

        jmethodID_performLogin = mEnv->GetStaticMethodID( mActivityClass, "facebookPerformLogin", "([Ljava/lang/String;)V" );
        jmethodID_getUser = mEnv->GetStaticMethodID( mActivityClass, "facebookGetUser", "()V" );
        jmethodID_shareLink = mEnv->GetStaticMethodID( mActivityClass, "facebookShareLink", "(Ljava/lang/String;)V" );
        jmethodID_isLoggedIn = mEnv->GetStaticMethodID( mActivityClass, "facebookIsLoggedIn", "()Z" );
        jmethodID_getProfilePictureLink = mEnv->GetStaticMethodID( mActivityClass, "facebookGetProfilePictureLink", "(Ljava/lang/String;)V" );
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

    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onProfilePictureLinkGet )(JNIEnv *env, jclass cls, jstring pictureURL_)
    {
        const char * pictureURL = env->GetStringUTFChars( pictureURL_, 0 );

        Mengine::FacebookProfilePictureURLCallbackPtr callback = g_currentFacebookProfilePictureURLCallback;
        g_currentFacebookProfilePictureURLCallback = nullptr;

        if( callback != nullptr )
        {
            callback->onProfilePictureLinkGet( pictureURL );
        }

        env->ReleaseStringUTFChars( pictureURL_, pictureURL );
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
                : m_cb( _cb )
                , m_args( _args )
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
        //////////////////////////////////////////////////////////////////////////
        bool androidFacebookIsLoggedIn( AndroidNativeFacebookPlugin * _plugin )
        {
            bool successful = _plugin->isLoggedIn();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonFacebookGetProfilePictureLinkCallback
            : public Callback<FacebookProfilePictureURLCallback>
        {
        public:
            PythonFacebookGetProfilePictureLinkCallback( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onProfilePictureLinkGet( const String & _pictureURL ) override
            {
                m_cb.call_args( 0, _pictureURL, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        bool androidFacebookGetProfilePictureLink( AndroidNativeFacebookPlugin * _plugin, const String & _typeParameter, const pybind::object & _cb, const pybind::args & _args )
        {
            bool successful = _plugin->getProfilePictureLink( _typeParameter
                , new FactorableUnique<PythonFacebookGetProfilePictureLinkCallback>( _cb, _args )
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
        pybind::def_function_proxy( kernel, "androidFacebookIsLoggedIn", &Detail::androidFacebookIsLoggedIn, this );
        pybind::def_function_proxy_args( kernel, "androidFacebookGetProfilePictureLink", &Detail::androidFacebookGetProfilePictureLink, this );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeFacebookPlugin::_finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookPlugin::isLoggedIn()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_isLoggedIn );

        bool returnValue = (bool)jReturnValue;

        return returnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookPlugin::performLogin( const TVectorString & _permissions, const FacebookLoginCallbackPtr & _callback )
    {
        if( g_currentFacebookLoginCallback != nullptr )
        {
            return false;
        }

        g_currentFacebookLoginCallback = _callback;

        JNIEnv *env = Mengine_JNI_GetEnv();

        if( _permissions.empty() == false )
        {
            TVectorString::size_type permissionsCount = _permissions.size();

            jclass jclass_string = env->FindClass( "java/lang/String" );
            jobjectArray jpermissions = env->NewObjectArray( permissionsCount, jclass_string, NULL );

            uint32_t jpermissionIterator = 0;
            for( const String & permission : _permissions )
            {
                const Char * permission_str = permission.c_str();

                jstring jpermission = env->NewStringUTF( permission_str );

                env->SetObjectArrayElement( jpermissions, jpermissionIterator++, jpermission );
            }

            env->CallStaticVoidMethod( mActivityClass, jmethodID_performLogin, jpermissions );
        }
        else
        {
            env->CallStaticVoidMethod( mActivityClass, jmethodID_performLogin, nullptr );
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

        JNIEnv * env = Mengine_JNI_GetEnv();

        env->CallStaticVoidMethod( mActivityClass, jmethodID_getUser );

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

        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * link_str = _link.c_str();
        jstring jlink = env->NewStringUTF( link_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_shareLink, jlink );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookPlugin::getProfilePictureLink( const String & _typeParameter, const FacebookProfilePictureURLCallbackPtr & _callback )
    {
        if( g_currentFacebookProfilePictureURLCallback != nullptr )
        {
            return false;
        }

        g_currentFacebookProfilePictureURLCallback = _callback;

        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * typeParameter_str = _typeParameter.c_str();
        jstring jtypeParameter = env->NewStringUTF( typeParameter_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_getProfilePictureLink, jtypeParameter );

        return true;
    }
}  

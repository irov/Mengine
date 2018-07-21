#include "AndroidNativeFacebookModule.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"

#include "Android/AndroidUtils.h"

#include "pybind/pybind.hpp"

#define FACEBOOK_JAVA_PREFIX org_Mengine_Build_Facebook
#define FACEBOOK_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(FACEBOOK_JAVA_PREFIX, FacebookInteractionLayer, function)

static jclass mActivityClass;
static jmethodID jmethodID_initializePlugin;
static jmethodID jmethodID_isLoggedIn;
static jmethodID jmethodID_performLogin;
static jmethodID jmethodID_getUser;
static jmethodID jmethodID_shareLink;
static jmethodID jmethodID_getProfilePictureLink;

static Mengine::AndroidNativeFacebookModule * s_androidNativeFacebookModule;

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeFacebook_1setupFacebookJNI )(JNIEnv *mEnv, jclass cls)
    {
        mActivityClass = (jclass)(mEnv->NewGlobalRef( cls ));

        jmethodID_initializePlugin = mEnv->GetStaticMethodID( mActivityClass, "facebookInitializePlugin", "()V" );
        jmethodID_performLogin = mEnv->GetStaticMethodID( mActivityClass, "facebookPerformLogin", "([Ljava/lang/String;)V" );
        jmethodID_getUser = mEnv->GetStaticMethodID( mActivityClass, "facebookGetUser", "()V" );
        jmethodID_shareLink = mEnv->GetStaticMethodID( mActivityClass, "facebookShareLink", "(Ljava/lang/String;)V" );
        jmethodID_isLoggedIn = mEnv->GetStaticMethodID( mActivityClass, "facebookIsLoggedIn", "()Z" );
        jmethodID_getProfilePictureLink = mEnv->GetStaticMethodID( mActivityClass, "facebookGetProfilePictureLink", "(Ljava/lang/String;)V" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeFacebook_1onSDKInitialized )(JNIEnv *mEnv, jclass cls)
    {
        if( s_androidNativeFacebookModule != nullptr )
        {
            s_androidNativeFacebookModule->addCommand( []( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookInitialized();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginSuccess )(JNIEnv *env, jclass cls, jstring accessToken_)
    {
        const char * accessToken = env->GetStringUTFChars( accessToken_, 0 );

        if( s_androidNativeFacebookModule != nullptr )
        {
            Mengine::String accessToken_str = accessToken;

            s_androidNativeFacebookModule->addCommand( [accessToken_str]( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookLoginSuccess( accessToken_str );
            } );
        }

        env->ReleaseStringUTFChars( accessToken_, accessToken );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginCancel )(JNIEnv *env, jclass cls)
    {
        if( s_androidNativeFacebookModule != nullptr )
        {
            s_androidNativeFacebookModule->addCommand( []( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookLoginCancel();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginError )(JNIEnv *env, jclass cls, jstring exception_)
    {
        const char * exception = env->GetStringUTFChars( exception_, 0 );

        if( s_androidNativeFacebookModule != nullptr )
        {
            Mengine::String exception_str = exception;

            s_androidNativeFacebookModule->addCommand( [exception_str]( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookLoginError( exception_str );
            } );
        }

        env->ReleaseStringUTFChars( exception_, exception );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onUserFetchSuccess )(JNIEnv *env, jclass cls, jstring object_, jstring response_)
    {
        const char * object = env->GetStringUTFChars( object_, 0 );
        const char * response = env->GetStringUTFChars( response_, 0 );

        if( s_androidNativeFacebookModule != nullptr )
        {
            Mengine::String object_str = object;
            Mengine::String response_str = response;

            s_androidNativeFacebookModule->addCommand( [object_str, response_str]( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookUserFetchSuccess( object_str, response_str );
            } );
        }

        env->ReleaseStringUTFChars( object_, object );
        env->ReleaseStringUTFChars( response_, response );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareSuccess )(JNIEnv *env, jclass cls, jstring postId_)
    {
        const char * postId = env->GetStringUTFChars( postId_, 0 );

        if( s_androidNativeFacebookModule != nullptr )
        {
            Mengine::String postId_str = postId;

            s_androidNativeFacebookModule->addCommand( [postId_str]( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookShareSuccess( postId_str );
            } );
        }

        env->ReleaseStringUTFChars( postId_, postId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareCancel )(JNIEnv *env, jclass cls)
    {
        if( s_androidNativeFacebookModule != nullptr )
        {
            s_androidNativeFacebookModule->addCommand( []( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookShareCancel();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareError )(JNIEnv *env, jclass cls, jstring exception_)
    {
        const char * exception = env->GetStringUTFChars( exception_, 0 );

        if( s_androidNativeFacebookModule != nullptr )
        {
            Mengine::String exception_str = exception;

            s_androidNativeFacebookModule->addCommand( [exception_str]( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookShareError( exception_str );
            } );
        }

        env->ReleaseStringUTFChars( exception_, exception );
    }

    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onProfilePictureLinkGet )(JNIEnv *env, jclass cls, jstring pictureURL_)
    {
        const char * pictureURL = env->GetStringUTFChars( pictureURL_, 0 );

        if( s_androidNativeFacebookModule != nullptr )
        {
            Mengine::String pictureURL_str = pictureURL;

            s_androidNativeFacebookModule->addCommand( [pictureURL_str]( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onProfilePictureLinkGet( pictureURL_str );
            } );
        }

        env->ReleaseStringUTFChars( pictureURL_, pictureURL );
    }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonFacebookEventHandler
            : public Callback<FacebookEventHandler>
        {
        public:
            PythonFacebookEventHandler( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onFacebookInitialized() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( FACEBOOK_INITIALIZE, pyparams, m_args );
            }

            void onFacebookLoginSuccess( const String & _token ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _token );

                m_cb.call_args( FACEBOOK_LOGIN_SUCCESS, pyparams, m_args );
            }

            void onFacebookLoginCancel() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( FACEBOOK_LOGIN_CANCEL, pyparams, m_args );
            }

            void onFacebookLoginError( const String & _exception ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _exception );

                m_cb.call_args( FACEBOOK_LOGIN_ERROR, pyparams, m_args );
            }

            void onFacebookShareSuccess( const String & _postId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _postId );

                m_cb.call_args( FACEBOOK_SHARE_SUCCESS, pyparams, m_args );
            }

            void onFacebookShareCancel() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( FACEBOOK_SHARE_CANCEL, pyparams, m_args );
            }

            void onFacebookShareError( const String & _exception ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _exception );

                m_cb.call_args( FACEBOOK_SHARE_ERROR, pyparams, m_args );
            }
            
            void onFacebookUserFetchSuccess( const String & _userObject, const String & _response ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _userObject, _response );

                m_cb.call_args( FACEBOOK_USER_FETCH_SUCCESS, pyparams, m_args );
            }

            void onProfilePictureLinkGet( const String & _pictureURL ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _pictureURL );

                m_cb.call_args( FACEBOOK_PROFILE_PICTURE_LINK_GET, pyparams, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static void androidFacebookSetEventHandler( AndroidNativeFacebookModule * _module, const pybind::object & _cb, const pybind::args & _args )
        {
            _module->setEventHandler(
                new FactorableUnique<PythonFacebookEventHandler>( _cb, _args )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeFacebookModule::AndroidNativeFacebookModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeFacebookModule::~AndroidNativeFacebookModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::_initialize()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function_proxy_args( kernel, "androidFacebookSetEventHandler", &Detail::androidFacebookSetEventHandler, this );

        pybind::def_functor( kernel, "androidFacebookIsLoggedIn", this, &AndroidNativeFacebookModule::isLoggedIn );
        pybind::def_functor( kernel, "androidFacebookPerformLogin", this, &AndroidNativeFacebookModule::performLogin );
        pybind::def_functor( kernel, "androidFacebookGetUser", this, &AndroidNativeFacebookModule::getUser );
        pybind::def_functor( kernel, "androidFacebookShareLink", this, &AndroidNativeFacebookModule::shareLink );
        pybind::def_functor( kernel, "androidFacebookGetProfilePictureLink", this, &AndroidNativeFacebookModule::getProfilePictureLink );

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

        m_eventation.setMutex( mutex );

        s_androidNativeFacebookModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeFacebookModule::_finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeFacebookModule::addCommand( const LambdaFacebookEventHandler & _command )
    {
        m_eventation.addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeFacebookModule::_update( bool _focus )
    {
        (void)_focus;

        m_eventation.invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeFacebookModule::setEventHandler( const FacebookEventHandlerPtr & _handler )
    {
        m_eventation.setEventHandler( _handler );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::initializeSDK()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        env->CallStaticVoidMethod( mActivityClass, jmethodID_initializePlugin );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::isLoggedIn()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_isLoggedIn );

        bool returnValue = (bool)jReturnValue;

        return returnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::performLogin( const TVectorString & _permissions )
    {
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
    bool AndroidNativeFacebookModule::getUser()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        env->CallStaticVoidMethod( mActivityClass, jmethodID_getUser );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::shareLink( const String & _link )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * link_str = _link.c_str();
        jstring jlink = env->NewStringUTF( link_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_shareLink, jlink );
        
        env->DeleteLocalRef( jlink );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::getProfilePictureLink( const String & _typeParameter )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * typeParameter_str = _typeParameter.c_str();
        jstring jtypeParameter = env->NewStringUTF( typeParameter_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_getProfilePictureLink, jtypeParameter );
        
        env->DeleteLocalRef( jtypeParameter );

        return true;
    }
}  
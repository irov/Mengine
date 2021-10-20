#include "AndroidNativeFacebookModule.h"

#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"

#include "Environment/Android/AndroidUtils.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "pybind/pybind.hpp"
#endif

#define FACEBOOK_JAVA_PREFIX org_Mengine_Build_Facebook
#define FACEBOOK_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(FACEBOOK_JAVA_PREFIX, FacebookInteractionLayer, function)

static jclass mActivityClass;
static jmethodID jmethodID_initializePlugin;
static jmethodID jmethodID_isLoggedIn;
static jmethodID jmethodID_getAccessToken;
static jmethodID jmethodID_performLogin;
static jmethodID jmethodID_logout;
static jmethodID jmethodID_getUser;
static jmethodID jmethodID_shareLink;
static jmethodID jmethodID_getProfilePictureLink;
static jmethodID jmethodID_getProfileUserPictureLink;

static Mengine::AndroidNativeFacebookModule * s_androidNativeFacebookModule;

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeFacebook_1setupFacebookJNI )(JNIEnv * env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_initializePlugin = env->GetStaticMethodID( mActivityClass, "facebookInitializePlugin", "()V" );
        jmethodID_performLogin = env->GetStaticMethodID( mActivityClass, "facebookPerformLogin", "([Ljava/lang/String;)Z" );
        jmethodID_logout = env->GetStaticMethodID( mActivityClass, "facebookLogout", "()Z" );
        jmethodID_getUser = env->GetStaticMethodID( mActivityClass, "facebookGetUser", "()Z" );
        jmethodID_shareLink = env->GetStaticMethodID( mActivityClass, "facebookShareLink", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z" );
        jmethodID_isLoggedIn = env->GetStaticMethodID( mActivityClass, "facebookIsLoggedIn", "()Z" );
        jmethodID_getAccessToken = env->GetStaticMethodID( mActivityClass, "facebookGetAccessToken", "()Ljava/lang/String;" );
        jmethodID_getProfilePictureLink = env->GetStaticMethodID( mActivityClass, "facebookGetProfilePictureLink", "(Ljava/lang/String;)Z" );
        jmethodID_getProfileUserPictureLink = env->GetStaticMethodID( mActivityClass, "facebookGetProfileUserPictureLink", "(Ljava/lang/String;Ljava/lang/String;)Z" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeFacebook_1onSDKInitialized )(JNIEnv * env, jclass cls)
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
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginSuccess )(JNIEnv * env, jclass cls, jstring accessToken_)
    {
        const Mengine::Char * accessToken = env->GetStringUTFChars( accessToken_, nullptr );

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
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginCancel )(JNIEnv * env, jclass cls)
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
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLoginError )(JNIEnv * env, jclass cls, jstring exception_)
    {
        const Mengine::Char * exception = env->GetStringUTFChars( exception_, nullptr );

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
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLogoutSuccess )(JNIEnv * env, jclass cls)
    {
        if( s_androidNativeFacebookModule != nullptr )
        {
            s_androidNativeFacebookModule->addCommand( []( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookLogoutSuccess();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLogoutCancel )(JNIEnv * env, jclass cls)
    {
        if( s_androidNativeFacebookModule != nullptr )
        {
            s_androidNativeFacebookModule->addCommand( []( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookLogoutCancel();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onLogoutError )(JNIEnv * env, jclass cls, jstring exception_)
    {
        const Mengine::Char * exception = env->GetStringUTFChars( exception_, nullptr );

        if( s_androidNativeFacebookModule != nullptr )
        {
            Mengine::String exception_str = exception;

            s_androidNativeFacebookModule->addCommand( [exception_str]( const Mengine::FacebookEventHandlerPtr & _handler )
            {
                _handler->onFacebookLogoutError( exception_str );
            } );
        }

        env->ReleaseStringUTFChars( exception_, exception );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onUserFetchSuccess )(JNIEnv * env, jclass cls, jstring object_, jstring response_)
    {
        const Mengine::Char * object = env->GetStringUTFChars( object_, nullptr );
        const Mengine::Char * response = env->GetStringUTFChars( response_, nullptr );

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
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareSuccess )(JNIEnv * env, jclass cls, jstring postId_)
    {
        const Mengine::Char * postId = env->GetStringUTFChars( postId_, nullptr );

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
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareCancel )(JNIEnv * env, jclass cls)
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
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onShareError )(JNIEnv * env, jclass cls, jstring exception_)
    {
        const Mengine::Char * exception = env->GetStringUTFChars( exception_, nullptr );

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
        FACEBOOK_JAVA_INTERFACE( AndroidNativeFacebook_1onProfilePictureLinkGet )(JNIEnv * env, jclass cls, jstring pictureURL_)
    {
        const Mengine::Char * pictureURL = env->GetStringUTFChars( pictureURL_, nullptr );

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
#ifdef MENGINE_USE_SCRIPT_SERVICE
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
            {
            }

            ~PythonFacebookEventHandler() override
            {
            }

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

            void onFacebookLogoutSuccess() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( FACEBOOK_LOGOUT_SUCCESS, pyparams, m_args );
            }

            void onFacebookLogoutCancel() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( FACEBOOK_LOGOUT_CANCEL, pyparams, m_args );
            }

            void onFacebookLogoutError( const String & _exception ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _exception );

                m_cb.call_args( FACEBOOK_LOGOUT_ERROR, pyparams, m_args );
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
                Helper::makeFactorableUnique<PythonFacebookEventHandler>( MENGINE_DOCUMENT_FUNCTION, _cb, _args )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeFacebookModule::AndroidNativeFacebookModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeFacebookModule::~AndroidNativeFacebookModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::_initializeModule()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

        pybind::enum_<EnumFacebookEventHandler>( kernel, "EnumFacebookEventHandler" )
            .def( "FACEBOOK_INITIALIZE", FACEBOOK_INITIALIZE )
            .def( "FACEBOOK_LOGIN_SUCCESS", FACEBOOK_LOGIN_SUCCESS )
            .def( "FACEBOOK_LOGIN_CANCEL", FACEBOOK_LOGIN_CANCEL )
            .def( "FACEBOOK_LOGIN_ERROR", FACEBOOK_LOGIN_ERROR )
            .def( "FACEBOOK_LOGOUT_SUCCESS", FACEBOOK_LOGOUT_SUCCESS )
            .def( "FACEBOOK_LOGOUT_CANCEL", FACEBOOK_LOGOUT_CANCEL )
            .def( "FACEBOOK_LOGOUT_ERROR", FACEBOOK_LOGOUT_ERROR )
            .def( "FACEBOOK_SHARE_SUCCESS", FACEBOOK_SHARE_SUCCESS )
            .def( "FACEBOOK_SHARE_CANCEL", FACEBOOK_SHARE_CANCEL )
            .def( "FACEBOOK_SHARE_ERROR", FACEBOOK_SHARE_ERROR )
            .def( "FACEBOOK_USER_FETCH_SUCCESS", FACEBOOK_USER_FETCH_SUCCESS )
            .def( "FACEBOOK_PROFILE_PICTURE_LINK_GET", FACEBOOK_PROFILE_PICTURE_LINK_GET )
            ;

        pybind::def_function_proxy_args( kernel, "androidFacebookSetEventHandler", &Detail::androidFacebookSetEventHandler, this );

        pybind::def_functor( kernel, "androidFacebookInitialize", this, &AndroidNativeFacebookModule::initializeSDK );
        pybind::def_functor( kernel, "androidFacebookIsLoggedIn", this, &AndroidNativeFacebookModule::isLoggedIn );
        pybind::def_functor( kernel, "androidFacebookGetAccessToken", this, &AndroidNativeFacebookModule::getAccessToken );
        pybind::def_functor( kernel, "androidFacebookPerformLogin", this, &AndroidNativeFacebookModule::performLogin );
        pybind::def_functor( kernel, "androidFacebookLogout", this, &AndroidNativeFacebookModule::logout );
        pybind::def_functor( kernel, "androidFacebookGetUser", this, &AndroidNativeFacebookModule::getUser );
        pybind::def_functor( kernel, "androidFacebookShareLink", this, &AndroidNativeFacebookModule::shareLink );
        pybind::def_functor( kernel, "androidFacebookGetProfilePictureLink", this, &AndroidNativeFacebookModule::getProfilePictureLink );
        pybind::def_functor( kernel, "androidFacebookGetProfileUserPictureLink", this, &AndroidNativeFacebookModule::getProfileUserPictureLink );
#endif

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation.initialize( mutex ) == false )
        {
            return false;
        }

        s_androidNativeFacebookModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeFacebookModule::_finalizeModule()
    {
        s_androidNativeFacebookModule = nullptr;

        m_eventation.finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeFacebookModule::addCommand( const LambdaFacebookEventHandler & _command )
    {
        m_eventation.addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeFacebookModule::_update( bool _focus )
    {
        MENGINE_UNUSED( _focus );

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
    bool AndroidNativeFacebookModule::isLoggedIn() const
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_isLoggedIn );

        bool returnValue = (bool)jReturnValue;

        return returnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    String AndroidNativeFacebookModule::getAccessToken() const
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jstring jReturnValue = (jstring)env->CallStaticObjectMethod( mActivityClass, jmethodID_getAccessToken );

        const Char * jStringValue = env->GetStringUTFChars( jReturnValue, nullptr );

        String stringValue = jStringValue;

        env->ReleaseStringUTFChars( jReturnValue, jStringValue );

        return stringValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::performLogin( const VectorString & _permissions )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        if( _permissions.empty() == true )
        {

            jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_performLogin, nullptr );

            return (bool)jReturnValue;
        }

        VectorString::size_type permissionsCount = _permissions.size();

        jclass jclass_string = env->FindClass( "java/lang/String" );
        jobjectArray jpermissions = env->NewObjectArray( permissionsCount, jclass_string, NULL );

        uint32_t jpermissionIterator = 0;
        for( const String & permission : _permissions )
        {
            const Char * permission_str = permission.c_str();

            jstring jpermission = env->NewStringUTF( permission_str );

            env->SetObjectArrayElement( jpermissions, jpermissionIterator++, jpermission );

            env->DeleteLocalRef( jpermission );
        }

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_performLogin, jpermissions );

        env->DeleteLocalRef( jpermissions );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::logout()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_logout );

        bool returnValue = (bool)jReturnValue;

        return returnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::getUser()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_getUser );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::shareLink( const String & _link, const String & _picture, const String & _message )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * link_str = _link.c_str();
        jstring jlink = env->NewStringUTF( link_str );
        const Char * picture_str = _picture.c_str();
        jstring jpicture = env->NewStringUTF( picture_str );
        const Char * message_str = _message.c_str();
        jstring jmessage = env->NewStringUTF( message_str );

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_shareLink, jlink, jpicture, jmessage );

        env->DeleteLocalRef( jlink );
        env->DeleteLocalRef( jpicture );
        env->DeleteLocalRef( jmessage );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::getProfilePictureLink( const String & _typeParameter )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * typeParameter_str = _typeParameter.c_str();
        jstring jtypeParameter = env->NewStringUTF( typeParameter_str );

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_getProfilePictureLink, jtypeParameter );

        env->DeleteLocalRef( jtypeParameter );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeFacebookModule::getProfileUserPictureLink( const String & _userId, const String & _typeParameter )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * userId_str = _userId.c_str();
        jstring juserId = env->NewStringUTF( userId_str );

        const Char * typeParameter_str = _typeParameter.c_str();
        jstring jtypeParameter = env->NewStringUTF( typeParameter_str );

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_getProfileUserPictureLink, juserId, jtypeParameter );

        env->DeleteLocalRef( juserId );
        env->DeleteLocalRef( jtypeParameter );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
}
#include "AndroidNativeUnityAdsModule.h"

#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidUtils.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "pybind/pybind.hpp"
#endif

#define UNITY_JAVA_PREFIX org_Mengine_Build_UnityAds
#define UNITY_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(UNITY_JAVA_PREFIX, UnityAdsInteractionLayer, function)

static jclass mActivityClass;
static jmethodID jmethodID_initializePlugin;
static jmethodID jmethodID_setupAds;
static jmethodID jmethodID_showAd;

static Mengine::AndroidNativeUnityAdsModule * s_androidNativeUnityModule = nullptr;

extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeUnity_1setupUnityJNI )(JNIEnv * env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_initializePlugin = env->GetStaticMethodID( mActivityClass, "unityInitializePlugin", "(Ljava/lang/String;)V" );
        jmethodID_setupAds = env->GetStaticMethodID( mActivityClass, "unitySetupAds", "(Z)Z" );
        jmethodID_showAd = env->GetStaticMethodID( mActivityClass, "unityShowAd", "(Ljava/lang/String;)Z" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeUnity_1onSDKInitialized )(JNIEnv * env, jclass cls)
    {
        if( s_androidNativeUnityModule != nullptr )
        {
            s_androidNativeUnityModule->addCommand( []( const Mengine::UnityAdsEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsInitialized();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsClick )(JNIEnv * env, jclass cls, jstring placementId_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String placementId_str = placementId;

            s_androidNativeUnityModule->addCommand( [placementId_str]( const Mengine::UnityAdsEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsClick( placementId_str );
            } );
        }

        env->ReleaseStringUTFChars( placementId_, placementId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsPlacementStateChanged )(JNIEnv * env,
            jclass cls, jstring placementId_, jint placementState_, jint placementState1_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );
        int placementState = static_cast<int>(placementState_);
        int placementState1 = static_cast<int>(placementState1_);

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String placementId_str = placementId;

            s_androidNativeUnityModule->addCommand( [placementId_str, placementState, placementState1]( const Mengine::UnityAdsEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsPlacementStateChanged( placementId_str, placementState, placementState1 );
            } );
        }

        env->ReleaseStringUTFChars( placementId_, placementId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsReady )(JNIEnv * env, jclass cls, jstring placementId_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String placementId_str = placementId;

            s_androidNativeUnityModule->addCommand( [placementId_str]( const Mengine::UnityAdsEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsReady( placementId_str );
            } );
        }

        env->ReleaseStringUTFChars( placementId_, placementId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsStart )(JNIEnv * env, jclass cls, jstring placementId_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String placementId_str = placementId;

            s_androidNativeUnityModule->addCommand( [placementId_str]( const Mengine::UnityAdsEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsStart( placementId_str );
            } );
        }

        env->ReleaseStringUTFChars( placementId_, placementId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsFinish )(JNIEnv * env, jclass cls, jstring placementId_, jint finishState_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );
        int finishState = static_cast<int>(finishState_);

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String placementId_str = placementId;

            s_androidNativeUnityModule->addCommand( [placementId_str, finishState]( const Mengine::UnityAdsEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsFinish( placementId_str, finishState );
            } );
        }

        env->ReleaseStringUTFChars( placementId_, placementId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsError )(JNIEnv * env, jclass cls,
            jint unityAdsError_, jstring message_)
    {
        int unityAdsError = static_cast<int>(unityAdsError_);
        const char * message = env->GetStringUTFChars( message_, 0 );

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String message_str = message;

            s_androidNativeUnityModule->addCommand( [unityAdsError, message_str]( const Mengine::UnityAdsEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsError( unityAdsError, message_str );
            } );
        }

        env->ReleaseStringUTFChars( message_, message );
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
        class PythonUnityAdsEventHandler
            : public Callback<UnityAdsEventHandler>
        {
        public:
            PythonUnityAdsEventHandler( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {
            }

            ~PythonUnityAdsEventHandler() override
            {
            }

        protected:
            void onUnityAdsInitialized() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( UNITYADS_INITIALIZE, pyparams, m_args );
            }

            void onUnityAdsReady( const String & _placementId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _placementId );

                m_cb.call_args( UNITYADS_READY, pyparams, m_args );
            }

            void onUnityAdsClick( const String & _placementId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _placementId );

                m_cb.call_args( UNITYADS_CLICK, pyparams, m_args );
            }

            void onUnityAdsPlacementStateChanged( const String & _placementId, int _placementState, int _placementState1 ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _placementId, _placementState, _placementState1 );

                m_cb.call_args( UNITYADS_PLACEMENT_STATE_CHANGED, pyparams, m_args );
            }

            void onUnityAdsStart( const String & _placementId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _placementId );

                m_cb.call_args( UNITYADS_START, pyparams, m_args );
            }

            void onUnityAdsFinish( const String & _placementId, int _finishState ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _placementId, _finishState );

                m_cb.call_args( UNITYADS_FINISH, pyparams, m_args );
            }

            void onUnityAdsError( int _unityAdsError, const String & _message ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _unityAdsError, _message );

                m_cb.call_args( UNITYADS_ERROR, pyparams, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static void androidUnitySetAdsEventHandler( AndroidNativeUnityAdsModule * _module, const pybind::object & _cb, const pybind::args & _args )
        {
            _module->setEventHandler(
                Helper::makeFactorableUnique<PythonUnityAdsEventHandler>( MENGINE_DOCUMENT_FUNCTION, _cb, _args )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeUnityAdsModule::AndroidNativeUnityAdsModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeUnityAdsModule::~AndroidNativeUnityAdsModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityAdsModule::_initializeModule()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

        pybind::enum_<EnumUnityAdsEventHandler>( kernel, "EnumUnityAdsEventHandler" )
                .def( "UNITYADS_INITIALIZE", UNITYADS_INITIALIZE )
                .def( "UNITYADS_READY", UNITYADS_READY )
                .def( "UNITYADS_CLICK", UNITYADS_CLICK )
                .def( "UNITYADS_PLACEMENT_STATE_CHANGED", UNITYADS_PLACEMENT_STATE_CHANGED )
                .def( "UNITYADS_START", UNITYADS_START )
                .def( "UNITYADS_FINISH", UNITYADS_FINISH )
                .def( "UNITYADS_ERROR", UNITYADS_ERROR )
                ;

        pybind::def_function_proxy_args( kernel, "androidUnitySetAdsEventHandler", &Detail::androidUnitySetAdsEventHandler, this );

        pybind::def_functor( kernel, "androidUnityInitialize", this, &AndroidNativeUnityAdsModule::initializeSDK );
        pybind::def_functor( kernel, "androidUnitySetupAds", this, &AndroidNativeUnityAdsModule::setupAds );
        pybind::def_functor( kernel, "androidUnityShowAd", this, &AndroidNativeUnityAdsModule::showAd );
#endif

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation.initialize( mutex ) == false )
        {
            return false;
        }

        s_androidNativeUnityModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityAdsModule::_finalizeModule()
    {
        s_androidNativeUnityModule = nullptr;

        m_eventation.finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityAdsModule::_update( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        m_eventation.invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityAdsModule::addCommand( const LambdaUnityAdsEventHandler & _command )
    {
        m_eventation.addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityAdsModule::initializeSDK( const String & _gameId )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * gameId_str = _gameId.c_str();
        jstring jgameId = env->NewStringUTF( gameId_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_initializePlugin, jgameId );

        env->DeleteLocalRef( jgameId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityAdsModule::setupAds( bool _debug )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jdebug = static_cast<jboolean>(_debug);

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_setupAds, jdebug );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityAdsModule::showAd( const String & _placementId )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * placementId_str = _placementId.c_str();
        jstring jplacementId = env->NewStringUTF( placementId_str );

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_showAd, jplacementId );

        env->DeleteLocalRef( jplacementId );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityAdsModule::setEventHandler( const UnityAdsEventHandlerPtr & _handler )
    {
        m_eventation.setEventHandler( _handler );
    }
    //////////////////////////////////////////////////////////////////////////
}

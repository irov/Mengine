#include "AndroidNativeUnityAdsModule.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"

#include "Android/AndroidUtils.h"

#include "pybind/pybind.hpp"

#include <jni.h>

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
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeUnity_1setupUnityJNI )(JNIEnv *env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_initializePlugin = env->GetStaticMethodID( mActivityClass, "unityInitializePlugin", "(Ljava/lang/String;)V" );
        jmethodID_setupAds = env->GetStaticMethodID( mActivityClass, "unitySetupAds", "(Z)V" );
        jmethodID_showAd = env->GetStaticMethodID( mActivityClass, "unityShowAd", "(Ljava/lang/String;)V" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsClick )(JNIEnv *env, jclass cls, jstring placementId_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String placementId_str = placementId;

            s_androidNativeUnityModule->addCommand( [placementId_str]( const Mengine::UnityAdEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsClick( placementId_str );
            } );            
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

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String placementId_str = placementId;

            s_androidNativeUnityModule->addCommand( [placementId_str, placementState, placementState1]( const Mengine::UnityAdEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsPlacementStateChanged( placementId_str, placementState, placementState1 );
            } );
        }

        env->ReleaseStringUTFChars( placementId_, placementId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsReady )(JNIEnv *env, jclass cls, jstring placementId_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String placementId_str = placementId;

            s_androidNativeUnityModule->addCommand( [placementId_str]( const Mengine::UnityAdEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsReady( placementId_str );
            } );
        }

        env->ReleaseStringUTFChars( placementId_, placementId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsStart )(JNIEnv *env, jclass cls, jstring placementId_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String placementId_str = placementId;

            s_androidNativeUnityModule->addCommand( [placementId_str]( const Mengine::UnityAdEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsStart( placementId_str );
            } );
        }

        env->ReleaseStringUTFChars( placementId_, placementId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsFinish )(JNIEnv *env, jclass cls, jstring placementId_, jint finishState_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );
        int finishState = static_cast<int>(finishState_);

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String placementId_str = placementId;

            s_androidNativeUnityModule->addCommand( [placementId_str, finishState]( const Mengine::UnityAdEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onUnityAdsFinish( placementId_str, finishState );
            } );
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

        if( s_androidNativeUnityModule != nullptr )
        {
            Mengine::String message_str = message;

            s_androidNativeUnityModule->addCommand( [unityAdsError, message_str]( const Mengine::UnityAdEventHandlerPtr & _eventHandler )
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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        enum EUnityAdsCommand
        {
            EUC_READY = 0,
            EUC_CLICK,
            EUC_CHANGED,
            EUC_START,
            EUC_FINISH,
            EUC_ERROR,
        };
        //////////////////////////////////////////////////////////////////////////
        static bool androidUnitySetupAds( AndroidNativeUnityAdsModule * _plugin, bool _debug )
        {
            bool successful = _plugin->setupAds( _debug );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool androidUnityShowAd( AndroidNativeUnityAdsModule * _plugin, const String & _placementId )
        {
            bool successful = _plugin->showAd( _placementId );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonUnityAdEventHandler
            : public Callback<UnityAdEventHandler>
        {
        public:
            PythonUnityAdEventHandler( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onUnityAdsReady( const String & _placementId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( _placementId, EUC_READY, pyparams, m_args );
            }

            void onUnityAdsClick( const String & _placementId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( _placementId, EUC_CLICK, pyparams, m_args );
            }

            void onUnityAdsPlacementStateChanged( const String & _placementId, int _placementState, int _placementState1 ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _placementState, _placementState1 );

                m_cb.call_args( _placementId, EUC_CHANGED, pyparams, m_args );
            }

            void onUnityAdsStart( const String & _placementId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( _placementId, EUC_START, pyparams, m_args );
            }

            void onUnityAdsFinish( const String & _placementId, int _finishState ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _finishState );

                m_cb.call_args( _placementId, EUC_FINISH, pyparams, m_args );
            }

            void onUnityAdsError( int _unityAdsError, const String & _message ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _unityAdsError, _message );

                m_cb.call_args( "", EUC_ERROR, pyparams, m_args );
            }
            
        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static void androidUnitySetAdsEventHandler( AndroidNativeUnityAdsModule * _module, const pybind::object & _cb, const pybind::args & _args )
        {
            _module->setAdsEventHandler(
                    new FactorableUnique<PythonUnityAdEventHandler>( _cb, _args )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeUnityAdsModule::AndroidNativeUnityAdsModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeUnityAdsModule::~AndroidNativeUnityAdsModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityAdsModule::_initialize()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function_proxy( kernel, "androidUnitySetupAds", &Detail::androidUnitySetupAds, this );
        pybind::def_function_proxy( kernel, "androidUnityShowAd", &Detail::androidUnityShowAd, this );
        pybind::def_function_proxy_args( kernel, "androidUnitySetAdsEventHandler", &Detail::androidUnitySetAdsEventHandler, this );

        m_mutex = THREAD_SERVICE()
                ->createMutex( __FILE__, __LINE__ );

        s_androidNativeUnityModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityAdsModule::_finalize()
    {
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityAdsModule::_update( bool _focus )
    {
        VectorUnityAdCommand commands;

        m_mutex->lock();
        std::swap( m_commands, commands );
        m_mutex->unlock();

        if( m_eventHandler == nullptr )
        {
            return;
        }

        for( const LambdaUnityAdEventHandler & command : commands )
        {
            command( m_eventHandler );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityAdsModule::addCommand( const LambdaUnityAdEventHandler & _command )
    {
        m_mutex->lock();
        m_commands.push_back( _command );
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityAdsModule::initializePlugin( const String & _gameId )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();
        
        const Char * gameId_str = _gameId.c_str();
        jstring jgameId = env->NewStringUTF( gameId_str );
        
        env->CallStaticVoidMethod( mActivityClass, jmethodID_initializePlugin, jgameId );
        
        env->ReleaseStringUTFChars( jgameId, gameId_str );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityAdsModule::setupAds( bool _debug )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();
        
        jboolean jdebug = static_cast<jboolean>(_debug);
        
        env->CallStaticVoidMethod( mActivityClass, jmethodID_setupAds, jdebug );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityAdsModule::showAd( const String & _placementId )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * placementId_str = _placementId.c_str();
        jstring jplacementId = env->NewStringUTF( placementId_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_showAd, jplacementId );
        
        env->ReleaseStringUTFChars( jplacementId, placementId_str );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityAdsModule::setAdsEventHandler( const UnityAdEventHandlerPtr & _eventHandler )
    {
        m_eventHandler = _eventHandler;
    }
}

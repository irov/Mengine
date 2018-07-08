#include "AndroidNativeUnityPlugin.h"

#include "Core/Callback.h"
#include "Android/AndroidUtils.h"

#include "pybind/pybind.hpp"

#include <jni.h>
#include <vector>

#define UNITY_JAVA_PREFIX org_Mengine_Build_UnityAds
#define UNITY_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(UNITY_JAVA_PREFIX, UnityAdsInteractionLayer, function)

static jclass mActivityClass;
static jmethodID jmethodID_setupAds;
static jmethodID jmethodID_showAd;

static Mengine::UnityAdEventHandlerPtr g_currentUnityAdEventHandler;

extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeUnity_1setupUnityJNI )(JNIEnv *env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_setupAds = env->GetStaticMethodID( mActivityClass, "unitySetupAds", "(Z)V" );
        jmethodID_showAd = env->GetStaticMethodID( mActivityClass, "unityShowAd", "(Ljava/lang/String;)V" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsClick )(JNIEnv *env, jclass cls, jstring placementId_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );

        if( g_currentUnityAdEventHandler != nullptr )
        {
            g_currentUnityAdEventHandler->onUnityAdsClick( placementId );
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

        if( g_currentUnityAdEventHandler != nullptr )
        {
            g_currentUnityAdEventHandler->onUnityAdsPlacementStateChanged( placementId, placementState, placementState1 );
        }

        env->ReleaseStringUTFChars( placementId_, placementId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsReady )(JNIEnv *env, jclass cls, jstring placementId_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );

        if( g_currentUnityAdEventHandler != nullptr )
        {
            g_currentUnityAdEventHandler->onUnityAdsReady( placementId );
        }

        env->ReleaseStringUTFChars( placementId_, placementId );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        UNITY_JAVA_INTERFACE( AndroidNativeUnity_1onUnityAdsStart )(JNIEnv *env, jclass cls, jstring placementId_)
    {
        const char * placementId = env->GetStringUTFChars( placementId_, 0 );

        if( g_currentUnityAdEventHandler != nullptr )
        {
            g_currentUnityAdEventHandler->onUnityAdsStart( placementId );
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

        if( g_currentUnityAdEventHandler != nullptr )
        {
            g_currentUnityAdEventHandler->onUnityAdsFinish( placementId, finishState );
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

        if( g_currentUnityAdEventHandler != nullptr )
        {
            g_currentUnityAdEventHandler->onUnityAdsError( unityAdsError, message );
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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool androidUnitySetupAds( AndroidNativeUnityPlugin * _plugin, bool _debug, const pybind::object & _cb, const pybind::args & _args )
        {
            bool successful = _plugin->setupAds( _debug );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool androidUnityShowAd( AndroidNativeUnityPlugin * _plugin, const String & _placementId )
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

                m_cb.call_args( _placementId, 0, m_args );
            }

            void onUnityAdsClick( const String & _placementId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( _placementId, 1, pyparams, m_args );
            }

            void onUnityAdsPlacementStateChanged( const String & _placementId, int _placementState, int _placementState1 ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _placementState, _placementState1 );

                m_cb.call_args( _placementId, 2, pyparams, m_args );
            }

            void onUnityAdsStart( const String & _placementId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( _placementId, 3, pyparams, m_args );
            }

            void onUnityAdsFinish( const String & _placementId, int _finishState ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _finishState );

                m_cb.call_args( _placementId, 4, pyparams, m_args );
            }

            void onUnityAdsError( int _unityAdsError, const String & _message ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _unityAdsError, _message );

                m_cb.call_args( "", 5, pyparams, m_args );
            }
            
        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static bool androidUnitySetAdsEventHandler( AndroidNativeUnityPlugin * _plugin, bool _debug, const pybind::object & _cb, const pybind::args & _args )
        {
            bool successful = _plugin->setAdsEventHandler(
                    new FactorableUnique<PythonUnityAdEventHandler>( _cb, _args )
            );

            return successful;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeUnityPlugin::AndroidNativeUnityPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeUnityPlugin::~AndroidNativeUnityPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityPlugin::_avaliable()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityPlugin::_initialize()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function_proxy( kernel, "androidUnitySetupAds", &Detail::androidUnitySetupAds, this );
        pybind::def_function_proxy( kernel, "androidUnityShowAd", &Detail::androidUnityShowAd, this );
        pybind::def_function_proxy_args( kernel, "androidUnitySetAdsEventHandler", &Detail::androidUnitySetAdsEventHandler, this );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityPlugin::_finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityPlugin::setupAds( bool _debug )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jdebug = static_cast<jboolean>(_debug);

        env->CallStaticVoidMethod( mActivityClass, jmethodID_setupAds, jdebug );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityPlugin::showAd( const String & _placementId )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * placementId_str = _placementId.c_str();
        jstring jplacementId = env->NewStringUTF( placementId_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_showAd, jplacementId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityPlugin::setAdsEventHandler( const UnityAdEventHandlerPtr & _callback )
    {
        if( g_currentUnityAdEventHandler != nullptr )
        {
            return false;
        }

        g_currentUnityAdEventHandler = _callback;

        return true;
    }
}

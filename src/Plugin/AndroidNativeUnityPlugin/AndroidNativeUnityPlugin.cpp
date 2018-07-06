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

static Mengine::UnitySetupCallbackPtr g_currentUnitySetupCallback;
static Mengine::UnityShowAdCallbackPtr g_currentUnityShowAdCallback;


extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeUnity_1setupUnityJNI )(JNIEnv *env, jclass cls)
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
    namespace Detail
    {   
        //////////////////////////////////////////////////////////////////////////
        class PythonUnitySetupCallback
            : public Callback<UnitySetupCallback>
        {
        public:
            PythonUnitySetupCallback( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onUnityAdsReady( const String & _placementId ) override
            {
                m_cb.call_args( 0, _placementId, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static bool androidUnitySetupAds( AndroidNativeUnityPlugin * _plugin, bool _debug, const pybind::object & _cb, const pybind::args & _args )
        {
            bool successful = _plugin->setupAds( _debug
                , new FactorableUnique<PythonUnitySetupCallback>( _cb, _args )
            );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonUnityShowAdCallback
            : public Callback<UnityShowAdCallback>
        {
        public:
            PythonUnityShowAdCallback( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onUnityAdsClick( const String & _placementId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( _placementId, 0, pyparams, m_args );
            }

            void onUnityAdsPlacementStateChanged( const String & _placementId, int _placementState, int _placementState1 ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _placementState, _placementState1 );

                m_cb.call_args( _placementId, 1, pyparams, m_args );
            }

            void onUnityAdsStart( const String & _placementId ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( _placementId, 2, pyparams, m_args );
            }

            void onUnityAdsFinish( const String & _placementId, int _finishState ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _finishState );

                m_cb.call_args( _placementId, 3, pyparams, m_args );
            }

            void onUnityAdsError( int _unityAdsError, const String & _message ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _unityAdsError, _message );

                m_cb.call_args( "", 4, pyparams, m_args );
            }
            
        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static bool androidUnityShowAd( AndroidNativeUnityPlugin * _plugin, const pybind::object & _cb, const pybind::args & _args )
        {
            bool successful = _plugin->showAd( new FactorableUnique<PythonUnityShowAdCallback>( _cb, _args )
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

        pybind::def_function_proxy_args( kernel, "androidUnitySetupAds", &Detail::androidUnitySetupAds, this );
        pybind::def_function_proxy_args( kernel, "androidUnityShowAd", &Detail::androidUnityShowAd, this );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityPlugin::_finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityPlugin::setupAds( bool _debug, const UnitySetupCallbackPtr & _callback )
    {
        if( g_currentUnitySetupCallback != nullptr )
        {
            return false;
        }

        g_currentUnitySetupCallback = _callback;

        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jdebug = static_cast<jboolean>(_debug);

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

        JNIEnv * env = Mengine_JNI_GetEnv();

        env->CallStaticVoidMethod( mActivityClass, jmethodID_showAd );

        return true;
    }
}

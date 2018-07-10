#include "AndroidNativeAdMobPlugin.h"

#include "Core/Callback.h"
#include "Android/AndroidUtils.h"

#include "pybind/pybind.hpp"

#include <jni.h>
#include <vector>

#define ADMOB_JAVA_PREFIX org_Mengine_Build_AdMob
#define ADMOB_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(ADMOB_JAVA_PREFIX, AdMobInteractionLayer, function)

static jclass mActivityClass;
static jmethodID jmethodID_setupInterstitialAd;
static jmethodID jmethodID_showInterstitialAd;
static jmethodID jmethodID_setupRewardedVideoAd;
static jmethodID jmethodID_showRewardedVideoAd;

static Mengine::AdMobInterstitialEventHandlerPtr g_currentAdMobInterstitialEventHandler;
static Mengine::AdMobRewardedVideoEventHandlerPtr g_currentAdMobRewardedVideoEventHandler;


extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeAdMob_1setupAdMobJNI )(JNIEnv *env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_setupInterstitialAd = env->GetStaticMethodID( mActivityClass, "admobSetupInterstitialAd", "()V" );
        jmethodID_showInterstitialAd = env->GetStaticMethodID( mActivityClass, "admobShowInterstitialAd", "()V" );
        jmethodID_setupRewardedVideoAd = env->GetStaticMethodID( mActivityClass, "admobSetupRewardedVideoAd", "()V" );
        jmethodID_showRewardedVideoAd = env->GetStaticMethodID( mActivityClass, "admobShowRewardedVideoAd", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdLoaded )( JNIEnv *env, jclass cls )
    {
        if( g_currentAdMobInterstitialEventHandler != nullptr )
        {
            g_currentAdMobInterstitialEventHandler->onAdLoaded();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdFailedToLoad )( JNIEnv *env, jclass cls, jint errorCode_ )
    {
        int errorCode = static_cast<int>(errorCode_);
        
        if( g_currentAdMobInterstitialEventHandler != nullptr )
        {
            g_currentAdMobInterstitialEventHandler->onAdFailedToLoad( errorCode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdOpened )( JNIEnv *env, jclass cls )
    {
        if( g_currentAdMobInterstitialEventHandler != nullptr )
        {
            g_currentAdMobInterstitialEventHandler->onAdOpened();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdLeftApplication )( JNIEnv *env, jclass cls )
    {
        if( g_currentAdMobInterstitialEventHandler != nullptr )
        {
            g_currentAdMobInterstitialEventHandler->onAdLeftApplication();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdClosed )( JNIEnv *env, jclass cls )
    {
        if( g_currentAdMobInterstitialEventHandler != nullptr )
        {
            g_currentAdMobInterstitialEventHandler->onAdClosed();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdLoaded )( JNIEnv *env, jclass cls )
    {
        if( g_currentAdMobRewardedVideoEventHandler != nullptr )
        {
            g_currentAdMobRewardedVideoEventHandler->onRewardedVideoAdLoaded();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdOpened )( JNIEnv *env, jclass cls )
    {
        if( g_currentAdMobRewardedVideoEventHandler != nullptr )
        {
            g_currentAdMobRewardedVideoEventHandler->onRewardedVideoAdOpened();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoStarted )( JNIEnv *env, jclass cls )
    {
        if( g_currentAdMobRewardedVideoEventHandler != nullptr )
        {
            g_currentAdMobRewardedVideoEventHandler->onRewardedVideoStarted();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdClosed )( JNIEnv *env, jclass cls )
    {
        if( g_currentAdMobRewardedVideoEventHandler != nullptr )
        {
            g_currentAdMobRewardedVideoEventHandler->onRewardedVideoAdClosed();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewarded )( JNIEnv *env, jclass cls, jstring rewardType_, jint rewardAmount_ )
    {
        const char * rewardType = env->GetStringUTFChars( rewardType_, 0 );
        int rewardAmount = static_cast<int>(rewardAmount_);
        
        if( g_currentAdMobRewardedVideoEventHandler != nullptr )
        {
            g_currentAdMobRewardedVideoEventHandler->onRewarded( rewardType, rewardAmount );
        }
        
        env->ReleaseStringUTFChars( rewardType_, rewardType );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
    ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdLeftApplication )( JNIEnv *env, jclass cls )
    {
        if( g_currentAdMobRewardedVideoEventHandler != nullptr )
        {
            g_currentAdMobRewardedVideoEventHandler->onRewardedVideoAdLeftApplication();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
    ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdFailedToLoad )( JNIEnv *env, jclass cls, jint errorCode_ )
    {
        int errorCode = static_cast<int>(errorCode_);
        
        if( g_currentAdMobRewardedVideoEventHandler != nullptr )
        {
            g_currentAdMobRewardedVideoEventHandler->onRewardedVideoAdFailedToLoad( errorCode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
    ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoCompleted )( JNIEnv *env, jclass cls )
    {
        if( g_currentAdMobRewardedVideoEventHandler != nullptr )
        {
            g_currentAdMobRewardedVideoEventHandler->onRewardedVideoCompleted();
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidNativeAdMob, Mengine::AndroidNativeAdMobPlugin )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {   
        //////////////////////////////////////////////////////////////////////////
        static bool androidAdMobSetupInterstitialAd( AndroidNativeAdMobPlugin * _plugin )
        {
            bool successful = _plugin->setupInterstitialAd();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool androidAdMobShowInterstitialAd( AndroidNativeAdMobPlugin * _plugin )
        {
            bool successful = _plugin->showInterstitialAd();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAdMobInterstitialEventHandler
            : public Callback<AdMobInterstitialEventHandler>
        {
        public:
            PythonAdMobInterstitialEventHandler( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onAdLoaded() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( 0, pyparams, m_args );
            }

            void onAdFailedToLoad( int _errorCode ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _errorCode );

                m_cb.call_args( 1, pyparams, m_args );
            }

            void onAdOpened() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( 2, pyparams, m_args );
            }

            void onAdLeftApplication() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( 3, pyparams, m_args );
            }

            void onAdClosed() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( 4, pyparams, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static bool androidAdMobSetInterstitialAdEventHandler( AndroidNativeAdMobPlugin * _plugin, const pybind::object & _cb, const pybind::args & _args )
        {
            bool successful = _plugin->setInterstitialEventHandler(
                new FactorableUnique<PythonAdMobInterstitialEventHandler>( _cb, _args )
            );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool androidAdMobSetupRewardedVideoAd( AndroidNativeAdMobPlugin * _plugin )
        {
            bool successful = _plugin->setupRewardedVideoAd();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool androidAdMobShowRewardedVideoAd( AndroidNativeAdMobPlugin * _plugin )
        {
            bool successful = _plugin->showRewardedVideoAd();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAdMobRewardedVideoEventHandler
            : public Callback<AdMobRewardedVideoEventHandler>
        {
        public:
            PythonAdMobRewardedVideoEventHandler( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onRewardedVideoAdLoaded() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( 0, pyparams, m_args );
            }

            void onRewardedVideoAdOpened() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( 1, pyparams, m_args );
            }

            void onRewardedVideoStarted() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( 2, pyparams, m_args );
            }

            void onRewardedVideoAdClosed() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( 3, pyparams, m_args );
            }

            void onRewarded( const String & _rewardType, int _rewardAmount ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _rewardType, _rewardAmount );

                m_cb.call_args( 4, pyparams, m_args );
            }

            void onRewardedVideoAdLeftApplication() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( 5, pyparams, m_args );
            }

            void onRewardedVideoAdFailedToLoad( int _errorCode ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _errorCode );

                m_cb.call_args( 6, pyparams, m_args );
            }

            void onRewardedVideoCompleted() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( 7, pyparams, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static bool androidAdMobSetRewardedVideoAdEventHandler( AndroidNativeAdMobPlugin * _plugin, const pybind::object & _cb, const pybind::args & _args )
        {
            bool successful = _plugin->setRewardedVideoEventHandler(
                new FactorableUnique<PythonAdMobRewardedVideoEventHandler>( _cb, _args )
            );

            return successful;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeAdMobPlugin::AndroidNativeAdMobPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeAdMobPlugin::~AndroidNativeAdMobPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobPlugin::_avaliable()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobPlugin::_initialize()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function_proxy( kernel, "androidAdMobSetupInterstitialAd", &Detail::androidAdMobSetupInterstitialAd, this );
        pybind::def_function_proxy( kernel, "androidAdMobShowInterstitialAd", &Detail::androidAdMobShowInterstitialAd, this );
        pybind::def_function_proxy_args( kernel, "androidAdMobSetInterstitialAdEventHandler", &Detail::androidAdMobSetInterstitialAdEventHandler, this );
        pybind::def_function_proxy( kernel, "androidAdMobSetupRewardedVideoAd", &Detail::androidAdMobSetupRewardedVideoAd, this );
        pybind::def_function_proxy( kernel, "androidAdMobShowRewardedVideoAd", &Detail::androidAdMobShowRewardedVideoAd, this );        
        pybind::def_function_proxy_args( kernel, "androidAdMobSetRewardedVideoAdEventHandler", &Detail::androidAdMobSetRewardedVideoAdEventHandler, this );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobPlugin::_finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobPlugin::setupInterstitialAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        env->CallStaticVoidMethod( mActivityClass, jmethodID_setupInterstitialAd );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobPlugin::showInterstitialAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        env->CallStaticVoidMethod( mActivityClass, jmethodID_showInterstitialAd );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobPlugin::setupRewardedVideoAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();
        
        env->CallStaticVoidMethod( mActivityClass, jmethodID_setupRewardedVideoAd );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobPlugin::showRewardedVideoAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();
        
        env->CallStaticVoidMethod( mActivityClass, jmethodID_showRewardedVideoAd );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobPlugin::setInterstitialEventHandler( const AdMobInterstitialEventHandlerPtr & _callback )
    {
        if( g_currentAdMobInterstitialEventHandler != nullptr )
        {
            return false;
        }
        
        g_currentAdMobInterstitialEventHandler = _callback;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobPlugin::setRewardedVideoEventHandler( const AdMobRewardedVideoEventHandlerPtr & _callback )
    {
        if( g_currentAdMobRewardedVideoEventHandler != nullptr )
        {
            return false;
        }
        
        g_currentAdMobRewardedVideoEventHandler = _callback;
        
        return true;
    }
}

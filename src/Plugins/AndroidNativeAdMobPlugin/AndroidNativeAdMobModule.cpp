#include "AndroidNativeAdMobModule.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"

#include "Environment/Android/AndroidUtils.h"

#include "pybind/pybind.hpp"

#include <jni.h>
#include <vector>

#define ADMOB_JAVA_PREFIX org_Mengine_Build_AdMob
#define ADMOB_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(ADMOB_JAVA_PREFIX, AdMobInteractionLayer, function)

static jclass mActivityClass;
static jmethodID jmethodID_initializePlugin;
static jmethodID jmethodID_setupInterstitialAd;
static jmethodID jmethodID_showInterstitialAd;
static jmethodID jmethodID_setupRewardedVideoAd;
static jmethodID jmethodID_showRewardedVideoAd;

static Mengine::AndroidNativeAdMobModule * s_androidNativeAdMobModule;

extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeAdMob_1setupAdMobJNI )(JNIEnv *env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_initializePlugin = env->GetStaticMethodID( mActivityClass, "admobInitializePlugin", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V" );
        jmethodID_setupInterstitialAd = env->GetStaticMethodID( mActivityClass, "admobSetupInterstitialAd", "()V" );
        jmethodID_showInterstitialAd = env->GetStaticMethodID( mActivityClass, "admobShowInterstitialAd", "()V" );
        jmethodID_setupRewardedVideoAd = env->GetStaticMethodID( mActivityClass, "admobSetupRewardedVideoAd", "()V" );
        jmethodID_showRewardedVideoAd = env->GetStaticMethodID( mActivityClass, "admobShowRewardedVideoAd", "()V" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeAdMob_1onSDKInitialized )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobInitialized();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdLoaded )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobInterstitialAdLoaded();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdFailedToLoad )( JNIEnv *env, jclass cls, jint errorCode_ )
    {
        int errorCode = static_cast<int>(errorCode_);
        
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( [errorCode]( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobInterstitialAdFailedToLoad( errorCode );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdOpened )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobInterstitialAdOpened();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdLeftApplication )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobInterstitialAdLeftApplication();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdClosed )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobInterstitialAdClosed();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdLoaded )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobRewardedVideoAdLoaded();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdOpened )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobRewardedVideoAdOpened();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoStarted )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobRewardedVideoAdStarted();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdClosed )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobRewardedVideoAdClosed();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewarded )( JNIEnv *env, jclass cls, jstring rewardType_, jint rewardAmount_ )
    {
        const char * rewardType = env->GetStringUTFChars( rewardType_, 0 );
        int rewardAmount = static_cast<int>(rewardAmount_);
        
        if( s_androidNativeAdMobModule != nullptr )
        {
            Mengine::String rewardType_str = rewardType;
            s_androidNativeAdMobModule->addCommand( [rewardType_str, rewardAmount]( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobRewardedVideoAdRewarded( rewardType_str, rewardAmount );
            } );
        }
        
        env->ReleaseStringUTFChars( rewardType_, rewardType );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
    ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdLeftApplication )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobRewardedVideoAdLeftApplication();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
    ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdFailedToLoad )( JNIEnv *env, jclass cls, jint errorCode_ )
    {
        int errorCode = static_cast<int>(errorCode_);
        
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( [errorCode]( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobRewardedVideoAdFailedToLoad( errorCode );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
    ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoCompleted )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobRewardedVideoAdCompleted();
            } );
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonAdMobEventHandler
            : public Callback<AdMobEventHandler>
        {
        public:
            PythonAdMobEventHandler( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onAdMobInitialized() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_INITIALIZE, pyparams, m_args );
            }

        protected:
            void onAdMobInterstitialAdLoaded() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_INTERSTITIALAD_LOAD, pyparams, m_args );
            }

            void onAdMobInterstitialAdFailedToLoad( int _errorCode ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _errorCode );

                m_cb.call_args( ADMOB_INTERSTITIALAD_FAILED_LOAD, pyparams, m_args );
            }

            void onAdMobInterstitialAdOpened() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_INTERSTITIALAD_OPENED, pyparams, m_args );
            }

            void onAdMobInterstitialAdLeftApplication() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_INTERSTITIALAD_LEFT_APPLICATION, pyparams, m_args );
            }

            void onAdMobInterstitialAdClosed() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_INTERSTITIALAD_CLOSED, pyparams, m_args );
            }

            void onAdMobRewardedVideoAdLoaded() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_LOAD, pyparams, m_args );
            }

            void onAdMobRewardedVideoAdOpened() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_OPENED, pyparams, m_args );
            }

            void onAdMobRewardedVideoAdStarted() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_STARTED, pyparams, m_args );
            }

            void onAdMobRewardedVideoAdClosed() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_CLOSED, pyparams, m_args );
            }

            void onAdMobRewardedVideoAdRewarded( const String & _rewardType, int _rewardAmount ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _rewardType, _rewardAmount );

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_REWARDED, pyparams, m_args );
            }

            void onAdMobRewardedVideoAdLeftApplication() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_LEFT_APPLICATION, pyparams, m_args );
            }

            void onAdMobRewardedVideoAdFailedToLoad( int _errorCode ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _errorCode );

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_FAILED_LOAD, pyparams, m_args );
            }

            void onAdMobRewardedVideoAdCompleted() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_COMPLETED, pyparams, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static void androidAdMobSetEventHandler( AndroidNativeAdMobModule * _module, const pybind::object & _cb, const pybind::args & _args )
        {
            _module->setEventHandler(
                new FactorableUnique<PythonAdMobEventHandler>( _cb, _args )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeAdMobModule::AndroidNativeAdMobModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeAdMobModule::~AndroidNativeAdMobModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::_initializeModule()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function_proxy_args( kernel, "androidAdMobSetEventHandler", &Detail::androidAdMobSetEventHandler, this );
        pybind::def_functor( kernel, "androidAdMobInitialize", this, &AndroidNativeAdMobModule::initializeSDK );        

        pybind::def_functor( kernel, "androidAdMobSetupInterstitialAd", this, &AndroidNativeAdMobModule::setupInterstitialAd );
        pybind::def_functor( kernel, "androidAdMobShowInterstitialAd", this, &AndroidNativeAdMobModule::showInterstitialAd );
        
        pybind::def_functor( kernel, "androidAdMobSetupRewardedVideoAd", this, &AndroidNativeAdMobModule::setupRewardedVideoAd );
        pybind::def_functor( kernel, "androidAdMobShowRewardedVideoAd", this, &AndroidNativeAdMobModule::showRewardedVideoAd );

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

        m_eventation.setMutex( mutex );

        s_androidNativeAdMobModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::_finalizeModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::_update( bool _focus )
    {
        (void)_focus;

        m_eventation.invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::addCommand( const LambdaAdMobEventHandler & _command )
    {
        m_eventation.addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::initializeSDK( const String & _admobAppId, const String & _interAdUnitId, const String & _videoAdUnitId )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();
        
        const Char * admobAppId_str = _admobAppId.c_str();
        jstring jadmobAppId = env->NewStringUTF( admobAppId_str );
        const Char * interAdUnitId_str = _interAdUnitId.c_str();
        jstring jinterAdUnitId = env->NewStringUTF( interAdUnitId_str );
        const Char * videoAdUnitId_str = _videoAdUnitId.c_str();
        jstring jvideoAdUnitId = env->NewStringUTF( videoAdUnitId_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_initializePlugin, jadmobAppId, jinterAdUnitId, jvideoAdUnitId );

        env->DeleteLocalRef( jadmobAppId );
        env->DeleteLocalRef( jinterAdUnitId );
        env->DeleteLocalRef( jvideoAdUnitId );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::setupInterstitialAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        env->CallStaticVoidMethod( mActivityClass, jmethodID_setupInterstitialAd );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::showInterstitialAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        env->CallStaticVoidMethod( mActivityClass, jmethodID_showInterstitialAd );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::setupRewardedVideoAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();
        
        env->CallStaticVoidMethod( mActivityClass, jmethodID_setupRewardedVideoAd );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::showRewardedVideoAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();
        
        env->CallStaticVoidMethod( mActivityClass, jmethodID_showRewardedVideoAd );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::setEventHandler( const AdMobEventHandlerPtr & _handler )
    {
        m_eventation.setEventHandler( _handler );
    }
}

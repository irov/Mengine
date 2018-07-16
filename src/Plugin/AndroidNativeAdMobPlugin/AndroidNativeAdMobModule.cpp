#include "AndroidNativeAdMobModule.h"

#include "Interface/ThreadSystemInterface.h"

#include "Core/Callback.h"
#include "Factory/FactorableUnique.h"

#include "Android/AndroidUtils.h"

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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdLoaded )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addInterstitialCommand( []( const Mengine::AdMobInterstitialEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdLoaded();
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
            s_androidNativeAdMobModule->addInterstitialCommand( [errorCode]( const Mengine::AdMobInterstitialEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdFailedToLoad( errorCode );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdOpened )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addInterstitialCommand( []( const Mengine::AdMobInterstitialEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdOpened();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdLeftApplication )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addInterstitialCommand( []( const Mengine::AdMobInterstitialEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdLeftApplication();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdClosed )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addInterstitialCommand( []( const Mengine::AdMobInterstitialEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdClosed();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdLoaded )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addRewardedVideoCommand( []( const Mengine::AdMobRewardedVideoEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onRewardedVideoAdLoaded();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdOpened )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addRewardedVideoCommand( []( const Mengine::AdMobRewardedVideoEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onRewardedVideoAdOpened();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoStarted )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addRewardedVideoCommand( []( const Mengine::AdMobRewardedVideoEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onRewardedVideoStarted();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdClosed )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addRewardedVideoCommand( []( const Mengine::AdMobRewardedVideoEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onRewardedVideoAdClosed();
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
            s_androidNativeAdMobModule->addRewardedVideoCommand( [rewardType_str, rewardAmount]( const Mengine::AdMobRewardedVideoEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onRewarded( rewardType_str, rewardAmount );
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
            s_androidNativeAdMobModule->addRewardedVideoCommand( []( const Mengine::AdMobRewardedVideoEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onRewardedVideoAdLeftApplication();
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
            s_androidNativeAdMobModule->addRewardedVideoCommand( [errorCode]( const Mengine::AdMobRewardedVideoEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onRewardedVideoAdFailedToLoad( errorCode );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
    ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoCompleted )( JNIEnv *env, jclass cls )
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addRewardedVideoCommand( []( const Mengine::AdMobRewardedVideoEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onRewardedVideoCompleted();
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
        static bool androidAdMobSetupInterstitialAd( AndroidNativeAdMobModule * _plugin )
        {
            bool successful = _plugin->setupInterstitialAd();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool androidAdMobShowInterstitialAd( AndroidNativeAdMobModule * _plugin )
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
        static void androidAdMobSetInterstitialAdEventHandler( AndroidNativeAdMobModule * _plugin, const pybind::object & _cb, const pybind::args & _args )
        {
            _plugin->setInterstitialEventHandler(
                new FactorableUnique<PythonAdMobInterstitialEventHandler>( _cb, _args )
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool androidAdMobSetupRewardedVideoAd( AndroidNativeAdMobModule * _plugin )
        {
            bool successful = _plugin->setupRewardedVideoAd();

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool androidAdMobShowRewardedVideoAd( AndroidNativeAdMobModule * _plugin )
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
        static void androidAdMobSetRewardedVideoAdEventHandler( AndroidNativeAdMobModule * _plugin, const pybind::object & _cb, const pybind::args & _args )
        {
            _plugin->setRewardedVideoEventHandler(
                new FactorableUnique<PythonAdMobRewardedVideoEventHandler>( _cb, _args )
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
    bool AndroidNativeAdMobModule::_initialize()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function_proxy( kernel, "androidAdMobSetupInterstitialAd", &Detail::androidAdMobSetupInterstitialAd, this );
        pybind::def_function_proxy( kernel, "androidAdMobShowInterstitialAd", &Detail::androidAdMobShowInterstitialAd, this );
        pybind::def_function_proxy_args( kernel, "androidAdMobSetInterstitialAdEventHandler", &Detail::androidAdMobSetInterstitialAdEventHandler, this );
        pybind::def_function_proxy( kernel, "androidAdMobSetupRewardedVideoAd", &Detail::androidAdMobSetupRewardedVideoAd, this );
        pybind::def_function_proxy( kernel, "androidAdMobShowRewardedVideoAd", &Detail::androidAdMobShowRewardedVideoAd, this );        
        pybind::def_function_proxy_args( kernel, "androidAdMobSetRewardedVideoAdEventHandler", &Detail::androidAdMobSetRewardedVideoAdEventHandler, this );

        m_mutex = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

        s_androidNativeAdMobModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::_finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::_update( bool _focus )
    {
        VectorAdMobInterstitialCommand interstitialCommands;
        VectorAdMobRewardedVideoCommand rewardedVideoCommands;

        m_mutex->lock();
        std::swap( m_interstitialCommands, interstitialCommands );
        std::swap( m_rewardedVideoCommands, rewardedVideoCommands );
        m_mutex->unlock();

        if( m_interstitialEventHandler != nullptr )
        {
            for( const LambdaAdMobInterstitialEventHandler & command : interstitialCommands )
            {
                command( m_interstitialEventHandler );
            }
        }

        if( m_rewardedVideoEventHandler != nullptr )
        {
            for( const LambdaAdMobRewardedVideoEventHandler & command : rewardedVideoCommands )
            {
                command( m_rewardedVideoEventHandler );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::addInterstitialCommand( const LambdaAdMobInterstitialEventHandler & _command )
    {
        m_mutex->lock();
        m_interstitialCommands.push_back( _command );
        m_mutex->unlock();
    }
    void AndroidNativeAdMobModule::addRewardedVideoCommand( const LambdaAdMobRewardedVideoEventHandler & _command )
    {
        m_mutex->lock();
        m_rewardedVideoCommands.push_back( _command );
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::initializePlugin( const String & _admobAppId, const String & _interAdUnitId, const String & _videoAdUnitId )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();
        
        const Char * admobAppId_str = _admobAppId.c_str();
        jstring jadmobAppId = env->NewStringUTF( admobAppId_str );
        const Char * interAdUnitId_str = _interAdUnitId.c_str();
        jstring jinterAdUnitId = env->NewStringUTF( interAdUnitId_str );
        const Char * videoAdUnitId_str = _videoAdUnitId.c_str();
        jstring jvideoAdUnitId = env->NewStringUTF( videoAdUnitId_str );
        
        env->CallStaticVoidMethod( mActivityClass, jmethodID_initializePlugin, jadmobAppId, jinterAdUnitId, jvideoAdUnitId );
        
        env->ReleaseStringUTFChars( jadmobAppId, admobAppId_str );
        env->ReleaseStringUTFChars( jinterAdUnitId, interAdUnitId_str );
        env->ReleaseStringUTFChars( jvideoAdUnitId, videoAdUnitId_str );
        
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
    void AndroidNativeAdMobModule::setInterstitialEventHandler( const AdMobInterstitialEventHandlerPtr & _callback )
    {       
        m_interstitialEventHandler = _callback;        
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::setRewardedVideoEventHandler( const AdMobRewardedVideoEventHandlerPtr & _callback )
    {
        m_rewardedVideoEventHandler = _callback;
    }
}

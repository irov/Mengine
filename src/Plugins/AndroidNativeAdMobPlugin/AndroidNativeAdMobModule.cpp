#include "AndroidNativeAdMobModule.h"

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

//////////////////////////////////////////////////////////////////////////
#define ADMOB_JAVA_PREFIX org_Mengine_Build_AdMob
#define ADMOB_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(ADMOB_JAVA_PREFIX, AdMobInteractionLayer, function)
//////////////////////////////////////////////////////////////////////////
static jclass mActivityClass;
static jmethodID jmethodID_initializePlugin;
static jmethodID jmethodID_setupInterstitialAd;
static jmethodID jmethodID_showInterstitialAd;
static jmethodID jmethodID_setupRewardedVideoAd;
static jmethodID jmethodID_showRewardedVideoAd;
//////////////////////////////////////////////////////////////////////////
static Mengine::AndroidNativeAdMobModule * s_androidNativeAdMobModule;
//////////////////////////////////////////////////////////////////////////
extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeAdMob_1setupAdMobJNI )(JNIEnv * env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_initializePlugin = env->GetStaticMethodID( mActivityClass, "admobInitializePlugin", "(Ljava/lang/String;Ljava/lang/String;)V" );
        jmethodID_setupInterstitialAd = env->GetStaticMethodID( mActivityClass, "admobSetupInterstitialAd", "()Z" );
        jmethodID_showInterstitialAd = env->GetStaticMethodID( mActivityClass, "admobShowInterstitialAd", "()Z" );
        jmethodID_setupRewardedVideoAd = env->GetStaticMethodID( mActivityClass, "admobSetupRewardedVideoAd", "()Z" );
        jmethodID_showRewardedVideoAd = env->GetStaticMethodID( mActivityClass, "admobShowRewardedVideoAd", "()Z" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeAdMob_1onSDKInitialized )(JNIEnv * env, jclass cls)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdLoaded )(JNIEnv * env, jclass cls)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdFailedToLoad )(JNIEnv * env, jclass cls, jint errorCode_)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdOpened )(JNIEnv * env, jclass cls)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdFailedToOpened )(JNIEnv * env, jclass cls)
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobInterstitialAdFailedToOpened();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdLeftApplication )(JNIEnv * env, jclass cls)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onAdClosed )(JNIEnv * env, jclass cls)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdLoaded )(JNIEnv * env, jclass cls)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdOpened )(JNIEnv * env, jclass cls)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdFailedToOpened )(JNIEnv * env, jclass cls)
    {
        if( s_androidNativeAdMobModule != nullptr )
        {
            s_androidNativeAdMobModule->addCommand( []( const Mengine::AdMobEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onAdMobRewardedVideoAdFailedToOpened();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoStarted )(JNIEnv * env, jclass cls)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdClosed )(JNIEnv * env, jclass cls)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewarded )(JNIEnv * env, jclass cls, jstring rewardType_, jint rewardAmount_)
    {
        const char * rewardType = env->GetStringUTFChars( rewardType_, nullptr );
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdLeftApplication )(JNIEnv * env, jclass cls)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoAdFailedToLoad )(JNIEnv * env, jclass cls, jint errorCode_)
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
        ADMOB_JAVA_INTERFACE( AndroidNativeAdMob_1onRewardedVideoCompleted )(JNIEnv * env, jclass cls)
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
#ifdef MENGINE_USE_SCRIPT_SERVICE
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
            {
            }

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

                m_cb.call_args( ADMOB_INTERSTITIALAD_LOADED, pyparams, m_args );
            }

            void onAdMobInterstitialAdFailedToLoad( int _errorCode ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _errorCode );

                m_cb.call_args( ADMOB_INTERSTITIALAD_FAILED_TO_LOAD, pyparams, m_args );
            }

            void onAdMobInterstitialAdOpened() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_INTERSTITIALAD_OPENED, pyparams, m_args );
            }

            void onAdMobInterstitialAdFailedToOpened() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_INTERSTITIALAD_FAILED_TO_OPENED, pyparams, m_args );
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

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_LOADED, pyparams, m_args );
            }

            void onAdMobRewardedVideoAdOpened() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_OPENED, pyparams, m_args );
            }

            void onAdMobRewardedVideoAdFailedToOpened() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_FAILED_TO_OPENED, pyparams, m_args );
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

                m_cb.call_args( ADMOB_REWARDEDVIDEOAD_FAILED_TO_LOAD, pyparams, m_args );
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
                Helper::makeFactorableUnique<PythonAdMobEventHandler>( MENGINE_DOCUMENT_FUNCTION, _cb, _args )
            );
        }
    }
#endif
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
#ifdef MENGINE_USE_SCRIPT_SERVICE
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

        pybind::enum_<EnumAdMobEventHandler>( kernel, "EnumAdMobEventHandler" )
            .def( "ADMOB_INITIALIZE", ADMOB_INITIALIZE )
            .def( "ADMOB_INTERSTITIALAD_LOADED", ADMOB_INTERSTITIALAD_LOADED )
            .def( "ADMOB_INTERSTITIALAD_FAILED_TO_LOAD", ADMOB_INTERSTITIALAD_FAILED_TO_LOAD )
            .def( "ADMOB_INTERSTITIALAD_OPENED", ADMOB_INTERSTITIALAD_OPENED )
            .def( "ADMOB_INTERSTITIALAD_FAILED_TO_OPENED", ADMOB_INTERSTITIALAD_FAILED_TO_OPENED )
            .def( "ADMOB_INTERSTITIALAD_LEFT_APPLICATION", ADMOB_INTERSTITIALAD_LEFT_APPLICATION )
            .def( "ADMOB_INTERSTITIALAD_CLOSED", ADMOB_INTERSTITIALAD_CLOSED )
            .def( "ADMOB_REWARDEDVIDEOAD_LOADED", ADMOB_REWARDEDVIDEOAD_LOADED )
            .def( "ADMOB_REWARDEDVIDEOAD_OPENED", ADMOB_REWARDEDVIDEOAD_OPENED )
            .def( "ADMOB_REWARDEDVIDEOAD_FAILED_TO_OPENED", ADMOB_REWARDEDVIDEOAD_FAILED_TO_OPENED )
            .def( "ADMOB_REWARDEDVIDEOAD_STARTED", ADMOB_REWARDEDVIDEOAD_STARTED )
            .def( "ADMOB_REWARDEDVIDEOAD_CLOSED", ADMOB_REWARDEDVIDEOAD_CLOSED )
            .def( "ADMOB_REWARDEDVIDEOAD_REWARDED", ADMOB_REWARDEDVIDEOAD_REWARDED )
            .def( "ADMOB_REWARDEDVIDEOAD_LEFT_APPLICATION", ADMOB_REWARDEDVIDEOAD_LEFT_APPLICATION )
            .def( "ADMOB_REWARDEDVIDEOAD_FAILED_TO_LOAD", ADMOB_REWARDEDVIDEOAD_FAILED_TO_LOAD )
            .def( "ADMOB_REWARDEDVIDEOAD_COMPLETED", ADMOB_REWARDEDVIDEOAD_COMPLETED )
            ;

        pybind::def_function_proxy_args( kernel, "androidAdMobSetEventHandler", &Detail::androidAdMobSetEventHandler, this );
        pybind::def_functor( kernel, "androidAdMobInitialize", this, &AndroidNativeAdMobModule::initializeSDK );

        pybind::def_functor( kernel, "androidAdMobSetupInterstitialAd", this, &AndroidNativeAdMobModule::setupInterstitialAd );
        pybind::def_functor( kernel, "androidAdMobShowInterstitialAd", this, &AndroidNativeAdMobModule::showInterstitialAd );

        pybind::def_functor( kernel, "androidAdMobSetupRewardedVideoAd", this, &AndroidNativeAdMobModule::setupRewardedVideoAd );
        pybind::def_functor( kernel, "androidAdMobShowRewardedVideoAd", this, &AndroidNativeAdMobModule::showRewardedVideoAd );
#endif

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation.initialize( mutex ) == false )
        {
            return false;
        }

        s_androidNativeAdMobModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::_finalizeModule()
    {
        s_androidNativeAdMobModule = nullptr;

        m_eventation.finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::_update( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        m_eventation.invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::addCommand( const LambdaAdMobEventHandler & _command )
    {
        m_eventation.addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::initializeSDK( const String & _interAdUnitId, const String & _videoAdUnitId )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * interAdUnitId_str = _interAdUnitId.c_str();
        jstring jinterAdUnitId = env->NewStringUTF( interAdUnitId_str );
        const Char * videoAdUnitId_str = _videoAdUnitId.c_str();
        jstring jvideoAdUnitId = env->NewStringUTF( videoAdUnitId_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_initializePlugin, jinterAdUnitId, jvideoAdUnitId );

        env->DeleteLocalRef( jinterAdUnitId );
        env->DeleteLocalRef( jvideoAdUnitId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::setupInterstitialAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_setupInterstitialAd );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::showInterstitialAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_showInterstitialAd );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::setupRewardedVideoAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_setupRewardedVideoAd );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeAdMobModule::showRewardedVideoAd()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_showRewardedVideoAd );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeAdMobModule::setEventHandler( const AdMobEventHandlerPtr & _handler )
    {
        m_eventation.setEventHandler( _handler );
    }
    //////////////////////////////////////////////////////////////////////////
}

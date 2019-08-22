#include "AndroidNativeDevToDevModule.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"

#include "Environment/Android/AndroidUtils.h"

#include "pybind/pybind.hpp"

#define DEVTODEV_JAVA_PREFIX org_Mengine_Build_DevToDev
#define DEVTODEV_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(DEVTODEV_JAVA_PREFIX, DevToDevInteractionLayer, function)

static jclass mActivityClass;
static jmethodID jmethodID_initializePlugin;
static jmethodID jmethodID_onTutorialEvent;
static jmethodID jmethodID_setCurrentLevel;
static jmethodID jmethodID_onLevelUp;
static jmethodID jmethodID_onCurrencyAccrual;
static jmethodID jmethodID_onRealPayment;
static jmethodID jmethodID_onInAppPurchase;
static jmethodID jmethodID_onSimpleCustomEvent;

static Mengine::AndroidNativeDevToDevModule * s_androidNativeDevToDevModule = nullptr;

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeDevToDev_1setupDevToDevJNI )(JNIEnv *mEnv, jclass cls)
    {
        mActivityClass = (jclass)(mEnv->NewGlobalRef( cls ));

        jmethodID_initializePlugin = mEnv->GetStaticMethodID( mActivityClass, "devtodevInitializePlugin", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V" );
        jmethodID_onTutorialEvent = mEnv->GetStaticMethodID( mActivityClass, "devtodevOnTutorialEvent", "(I)V" );
        jmethodID_setCurrentLevel = mEnv->GetStaticMethodID( mActivityClass, "devtodevSetCurrentLevel", "(I)V" );
        jmethodID_onLevelUp = mEnv->GetStaticMethodID( mActivityClass, "devtodevOnLevelUp", "(I)V" );
        jmethodID_onCurrencyAccrual = mEnv->GetStaticMethodID( mActivityClass, "devtodevOnCurrencyAccrual", "(Ljava/lang/String;II)V" );
        jmethodID_onRealPayment = mEnv->GetStaticMethodID( mActivityClass, "devtodevOnRealPayment", "(Ljava/lang/String;FLjava/lang/String;Ljava/lang/String;)V" );
        jmethodID_onInAppPurchase = mEnv->GetStaticMethodID( mActivityClass, "devtodevOnInAppPurchase", "(Ljava/lang/String;Ljava/lang/String;IILjava/lang/String;)V" );
        jmethodID_onSimpleCustomEvent = mEnv->GetStaticMethodID( mActivityClass, "devtodevOnSimpleCustomEvent", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeDevToDev_1onSDKInitialized )(JNIEnv *mEnv, jclass cls)
    {
        mActivityClass = (jclass)(mEnv->NewGlobalRef( cls ));

        if( s_androidNativeDevToDevModule != nullptr )
        {
            s_androidNativeDevToDevModule->addCommand( []( const Mengine::DevToDevEventHandlerPtr & _handler )
            {
                _handler->onDevToDevInitialized();
            } );
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonDevToDevEventHandler
            : public Callback<DevToDevEventHandler>
        {
        public:
            PythonDevToDevEventHandler( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            void onDevToDevInitialized() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( DEVTODEV_INITIALIZE, pyparams, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static void androidDevToDevSetEventHandler( AndroidNativeDevToDevModule * _module, const pybind::object & _cb, const pybind::args & _args )
        {
            _module->setEventHandler(
                Helper::makeFactorableUnique<PythonDevToDevEventHandler>( _cb, _args )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeDevToDevModule::AndroidNativeDevToDevModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeDevToDevModule::~AndroidNativeDevToDevModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeDevToDevModule::_initializeModule()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function_proxy_args( kernel, "androidDevToDevSetEventHandler", &Detail::androidDevToDevSetEventHandler, this );
        pybind::def_functor( kernel, "androidDevToDevInitialize", this, &AndroidNativeDevToDevModule::initializeSDK );
        pybind::def_functor( kernel, "androidDevToDevSetTutorialEvent", this, &AndroidNativeDevToDevModule::onTutorialEvent );
        pybind::def_functor( kernel, "androidDevToDevSetCurrentLevel", this, &AndroidNativeDevToDevModule::setCurrentLevel );
        pybind::def_functor( kernel, "androidDevToDevOnLevelUp", this, &AndroidNativeDevToDevModule::onLevelUp );
        pybind::def_functor( kernel, "androidDevToDevOnCurrencyAccrual", this, &AndroidNativeDevToDevModule::onCurrencyAccrual );
        pybind::def_functor( kernel, "androidDevToDevOnRealPayment", this, &AndroidNativeDevToDevModule::onRealPayment );
        pybind::def_functor( kernel, "androidDevToDevOnInAppPurchase", this, &AndroidNativeDevToDevModule::onInAppPurchase );
        pybind::def_functor( kernel, "androidDevToDevOnSimpleCustomEvent", this, &AndroidNativeDevToDevModule::onSimpleCustomEvent );

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        m_eventation.setMutex( mutex );

        s_androidNativeDevToDevModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeDevToDevModule::_finalizeModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeDevToDevModule::addCommand( const LambdaDevToDevEventHandler & _command )
    {
        m_eventation.addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeDevToDevModule::_update( bool _focus )
    {
        (void)_focus;

        m_eventation.invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeDevToDevModule::setEventHandler( const DevToDevEventHandlerPtr & _handler )
    {
        m_eventation.setEventHandler( _handler );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeDevToDevModule::initializeSDK( const String & _appId, const String & _secret, const String & _apiKey )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * appId_str = _appId.c_str();
        jstring jappId = env->NewStringUTF( appId_str );
        const Char * secret_str = _secret.c_str();
        jstring jsecret = env->NewStringUTF( secret_str );
        const Char * apiKey_str = _apiKey.c_str();
        jstring japiKey = env->NewStringUTF( apiKey_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_initializePlugin, jappId, jsecret, japiKey );

        env->DeleteLocalRef( jappId );
        env->DeleteLocalRef( jsecret );
        env->DeleteLocalRef( japiKey );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeDevToDevModule::onTutorialEvent( int _stateOrStep )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jint jstateOrStep = static_cast<jint>(_stateOrStep);

        env->CallStaticVoidMethod( mActivityClass, jmethodID_onTutorialEvent, jstateOrStep );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeDevToDevModule::setCurrentLevel( int _level )
    {
        JNIEnv *env = Mengine_JNI_GetEnv();

        jint jlevel = static_cast<jint>(_level);

        env->CallStaticVoidMethod( mActivityClass, jmethodID_setCurrentLevel, jlevel );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeDevToDevModule::onLevelUp( int _level )
    {
        JNIEnv *env = Mengine_JNI_GetEnv();

        jint jlevel = static_cast<jint>(_level);

        env->CallStaticVoidMethod( mActivityClass, jmethodID_onLevelUp, jlevel );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeDevToDevModule::onCurrencyAccrual( const String & _currencyName, int _currencyAmount, int _accrualType )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * currencyName_str = _currencyName.c_str();
        jstring jcurrencyName = env->NewStringUTF( currencyName_str );
        jint jcurrencyAmount = static_cast<jint>(_currencyAmount);
        jint jaccrualType = static_cast<jint>(_accrualType);

        env->CallStaticVoidMethod( mActivityClass, jmethodID_onCurrencyAccrual, jcurrencyName, jcurrencyAmount, jaccrualType );

        env->DeleteLocalRef( jcurrencyName );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeDevToDevModule::onRealPayment( const String & _paymentId, float _inAppPrice, const String & _inAppName, const String & _inAppCurrencyISOCode )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * paymentId_str = _paymentId.c_str();
        jstring jpaymentId = env->NewStringUTF( paymentId_str );
        jfloat jinAppPrice = static_cast<jfloat>(_inAppPrice);
        const Char * inAppName_str = _inAppName.c_str();
        jstring jinAppName = env->NewStringUTF( inAppName_str );
        const Char * inAppCurrencyISOCode_str = _inAppCurrencyISOCode.c_str();
        jstring jinAppCurrencyISOCode = env->NewStringUTF( inAppCurrencyISOCode_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_onRealPayment, jpaymentId, jinAppPrice, jinAppName, jinAppCurrencyISOCode );

        env->DeleteLocalRef( jpaymentId );
        env->DeleteLocalRef( jinAppName );
        env->DeleteLocalRef( jinAppCurrencyISOCode );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeDevToDevModule::onInAppPurchase( const String & _purchaseId, const String & _purchaseType, int _purchaseAmount, int _purchasePrice, const String & _purchaseCurrency )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * purchaseId_str = _purchaseId.c_str();
        jstring jpurchaseId = env->NewStringUTF( purchaseId_str );
        const Char * purchaseType_str = _purchaseType.c_str();
        jstring jpurchaseType = env->NewStringUTF( purchaseType_str );
        jint jpurchaseAmount = static_cast<jint>(_purchaseAmount);
        jint jpurchasePrice = static_cast<jint>(_purchasePrice);
        const Char * purchaseCurrency_str = _purchaseCurrency.c_str();
        jstring jpurchaseCurrency = env->NewStringUTF( purchaseCurrency_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_onInAppPurchase, jpurchaseId, jpurchaseType, jpurchaseAmount, jpurchasePrice, jpurchaseCurrency );

        env->DeleteLocalRef( jpurchaseId );
        env->DeleteLocalRef( jpurchaseType );
        env->DeleteLocalRef( jpurchaseCurrency );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeDevToDevModule::onSimpleCustomEvent( const String & _eventName, const String & _intJSON, const String & _floatJSON, const String & _stringJSON )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * eventName_str = _eventName.c_str();
        jstring jeventName = env->NewStringUTF( eventName_str );
        const Char * intJSON_str = _intJSON.c_str();
        jstring jintJSON = env->NewStringUTF( intJSON_str );
        const Char * floatJSON_str = _floatJSON.c_str();
        jstring jfloatJSON = env->NewStringUTF( floatJSON_str );
        const Char * stringJSON_str = _stringJSON.c_str();
        jstring jstringJSON = env->NewStringUTF( stringJSON_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_onSimpleCustomEvent, jeventName, jintJSON, jfloatJSON, jstringJSON );

        env->DeleteLocalRef( jeventName );
        env->DeleteLocalRef( jintJSON );
        env->DeleteLocalRef( jfloatJSON );
        env->DeleteLocalRef( jstringJSON );

        return true;
    }
}  
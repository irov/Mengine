#include "AndroidNativeLocalNotificationsModule.h"

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

static jclass mActivityClass;
static jmethodID jmethodID_localNotificationsInitializePlugin;
static jmethodID jmethodID_scheduleLocalNotification;
static jmethodID jmethodID_instantlyPresentLocalNotification;
static jmethodID jmethodID_cancelAllLocalNotification;

static Mengine::AndroidNativeLocalNotificationsModule * s_androidNativeLocalNotificationsModule;

extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeLocalNotifications_1setupLocalNotificationsJNI )(JNIEnv * env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_localNotificationsInitializePlugin = env->GetStaticMethodID( mActivityClass, "localNotificationsInitializePlugin", "()V" );
        jmethodID_scheduleLocalNotification = env->GetStaticMethodID( mActivityClass, "scheduleLocalNotification", "(ILjava/lang/String;Ljava/lang/String;I)Z" );
        jmethodID_instantlyPresentLocalNotification = env->GetStaticMethodID( mActivityClass, "instantlyPresentLocalNotification", "(ILjava/lang/String;Ljava/lang/String;)Z" );
        jmethodID_cancelAllLocalNotification = env->GetStaticMethodID( mActivityClass, "cancelAllLocalNotification", "()Z" );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeLocalNotifications_1onLocalNotificationsInitialized )(JNIEnv * env, jclass cls)
    {
        if( s_androidNativeLocalNotificationsModule != nullptr )
        {
            s_androidNativeLocalNotificationsModule->addCommand( []( const Mengine::LocalNotificationsEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onLocalNotificationsInitialized();
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeLocalNotifications_1onLocalNotificationsPress )(JNIEnv * env, jclass cls, jint _id)
    {
        int id = static_cast<int>(_id);

        if( s_androidNativeLocalNotificationsModule != nullptr )
        {
            s_androidNativeLocalNotificationsModule->addCommand( [id]( const Mengine::LocalNotificationsEventHandlerPtr & _eventHandler )
            {
                _eventHandler->onLocalNotificationsPress( id );
            } );
        }
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
        class PythonLocalNotificationsEventHandler
            : public Callback<LocalNotificationsEventHandler>
        {
        public:
            PythonLocalNotificationsEventHandler( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {
            }

            ~PythonLocalNotificationsEventHandler() override
            {
            }

        protected:
            void onLocalNotificationsInitialized() override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel() );

                m_cb.call_args( LOCAL_NOTIFICATIONS_INITIALIZE, pyparams, m_args );
            }

        protected:
            void onLocalNotificationsPress( int _id ) override
            {
                pybind::tuple pyparams = pybind::make_tuple_t( m_cb.kernel(), _id );

                m_cb.call_args( LOCAL_NOTIFICATIONS_PRESS, pyparams, m_args );
            }

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static void androidLocalNotificationsSetEventHandler( AndroidNativeLocalNotificationsModule * _module, const pybind::object & _cb, const pybind::args & _args )
        {
            _module->setEventHandler(
                Helper::makeFactorableUnique<PythonLocalNotificationsEventHandler>( MENGINE_DOCUMENT_FUNCTION, _cb, _args )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeLocalNotificationsModule::AndroidNativeLocalNotificationsModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeLocalNotificationsModule::~AndroidNativeLocalNotificationsModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLocalNotificationsModule::_initializeModule()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

        pybind::enum_<EnumLocalNotificationsEventHandler>( kernel, "EnumLocalNotificationsEventHandler" )
                .def( "LOCAL_NOTIFICATIONS_INITIALIZE", LOCAL_NOTIFICATIONS_INITIALIZE )
                .def( "LOCAL_NOTIFICATIONS_PRESS", LOCAL_NOTIFICATIONS_PRESS )
                ;

        pybind::def_function_proxy_args( kernel, "androidLocalNotificationsSetEventHandler", &Detail::androidLocalNotificationsSetEventHandler, this );
        pybind::def_functor( kernel, "androidInitializeLocalNotifications", this, &AndroidNativeLocalNotificationsModule::initializeLocalNotifications );

        pybind::def_functor( kernel, "androidScheduleLocalNotification", this, &AndroidNativeLocalNotificationsModule::scheduleLocalNotification );
        pybind::def_functor( kernel, "androidInstantlyPresentLocalNotification", this, &AndroidNativeLocalNotificationsModule::instantlyPresentLocalNotification );
        pybind::def_functor( kernel, "cancelAllLocalNotification", this, &AndroidNativeLocalNotificationsModule::cancelAllLocalNotification );
#endif

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation.initialize( mutex ) == false )
        {
            return false;
        }

        s_androidNativeLocalNotificationsModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLocalNotificationsModule::_finalizeModule()
    {
        s_androidNativeLocalNotificationsModule = nullptr;

        m_eventation.finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLocalNotificationsModule::_update( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        m_eventation.invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLocalNotificationsModule::setEventHandler( const LocalNotificationsEventHandlerPtr & _handler )
    {
        m_eventation.setEventHandler( _handler );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLocalNotificationsModule::addCommand( const LambdaLocalNotificationsEventHandler & _command )
    {
        m_eventation.addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLocalNotificationsModule::initializeLocalNotifications()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        env->CallStaticVoidMethod( mActivityClass, jmethodID_localNotificationsInitializePlugin );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLocalNotificationsModule::scheduleLocalNotification( int _id, const String & _title, const String & _content, int _delay )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jint jid = static_cast<jint>(_id);

        const Char * title_str = _title.c_str();
        jstring jtitle = env->NewStringUTF( title_str );

        const Char * content_str = _content.c_str();
        jstring jcontent = env->NewStringUTF( content_str );

        jint jdelay = static_cast<jint>(_delay * 1000);

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_scheduleLocalNotification, jid, jtitle, jcontent, jdelay );

        env->DeleteLocalRef( jtitle );
        env->DeleteLocalRef( jcontent );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLocalNotificationsModule::instantlyPresentLocalNotification( int _id, const String & _title, const String & _content )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jint jid = static_cast<jint>(_id);

        const Char * title_str = _title.c_str();
        jstring jtitle = env->NewStringUTF( title_str );

        const Char * content_str = _content.c_str();
        jstring jcontent = env->NewStringUTF( content_str );

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_instantlyPresentLocalNotification, jid, jtitle, jcontent );

        env->DeleteLocalRef( jtitle );
        env->DeleteLocalRef( jcontent );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLocalNotificationsModule::cancelAllLocalNotification()
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_cancelAllLocalNotification );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
}

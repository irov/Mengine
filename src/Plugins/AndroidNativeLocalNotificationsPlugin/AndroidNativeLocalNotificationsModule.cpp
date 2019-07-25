#include "AndroidNativeLocalNotificationsModule.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"

#include "Environment/Android/AndroidUtils.h"

#include "pybind/pybind.hpp"

#include <jni.h>
#include <vector>

#define LOCAL_NOTIFICATIONS_JAVA_PREFIX org_Mengine_Build_LocalNotifications
#define LOCAL_NOTIFICATIONS_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(LOCAL_NOTIFICATIONS_JAVA_PREFIX, NotificationPublisher, function)

static jclass mActivityClass;
static jmethodID jmethodID_localNotificationsInitializePlugin;
static jmethodID jmethodID_scheduleLocalNotification;
static jmethodID jmethodID_instantlyPresentLocalNotification;


static Mengine::AndroidNativeLocalNotificationsModule * s_androidNativeLocalNotificationsModule;

extern "C" {
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeLocalNotifications_1setupLocalNotificationsJNI )(JNIEnv *env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_localNotificationsInitializePlugin = env->GetStaticMethodID( mActivityClass, "localNotificationsInitializePlugin", "()V" );
        jmethodID_scheduleLocalNotification = env->GetStaticMethodID( mActivityClass, "scheduleLocalNotification", "(ILjava/lang/String;Ljava/lang/String;I)V" );
        jmethodID_instantlyPresentLocalNotification = env->GetStaticMethodID( mActivityClass, "instantlyPresentLocalNotification", "(ILjava/lang/String;Ljava/lang/String;)V" );

    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeLocalNotifications_1onLocalNotificationsInitialized )( JNIEnv *env, jclass cls )
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
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeLocalNotifications_1onLocalNotificationsPress )( JNIEnv *env, jclass cls, jint _id )
    {
        int id = static_cast<int>(_id);

        if( s_androidNativeLocalNotificationsModule != nullptr )
        {
            s_androidNativeLocalNotificationsModule->addCommand( [ id ]( const Mengine::LocalNotificationsEventHandlerPtr & _eventHandler )
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
            {}

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
                Helper::makeFactorableUnique<PythonLocalNotificationsEventHandler>( _cb, _args )
            );
        }
    }
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
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function_proxy_args( kernel, "androidLocalNotificationsSetEventHandler", &Detail::androidLocalNotificationsSetEventHandler, this );
        pybind::def_functor( kernel, "androidInitializeLocalNotifications", this, &AndroidNativeLocalNotificationsModule::initializeLocalNotifications );

        pybind::def_functor( kernel, "androidScheduleLocalNotification", this, &AndroidNativeLocalNotificationsModule::scheduleLocalNotification );
        pybind::def_functor( kernel, "androidInstantlyPresentLocalNotification", this, &AndroidNativeLocalNotificationsModule::instantlyPresentLocalNotification );

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        m_eventation.setMutex( mutex );

        s_androidNativeLocalNotificationsModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLocalNotificationsModule::_finalizeModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLocalNotificationsModule::_update( bool _focus )
    {
        (void)_focus;

        m_eventation.invoke();
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
    bool AndroidNativeLocalNotificationsModule::scheduleLocalNotification( int _id, const WString & _title, const WString & _content, int _delay )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jint jid = static_cast<jint>(_id);
        const wchar_t * title_str = _title.c_str();
        size_t titleLength = _title.length();
        jchar* title_jchar = (jchar*)malloc((titleLength + 1)*sizeof(jchar));
        for (int i = 0; i < titleLength; i++)
            title_jchar[i] = (jchar) title_str[i];
        title_jchar[titleLength] = 0;
        jstring jtitle = env->NewString( title_jchar, titleLength );
        const wchar_t * content_str = _content.c_str();
        size_t contentLength = _content.length();
        jchar* content_jchar = (jchar*)malloc((contentLength + 1)*sizeof(jchar));
        for (int i = 0; i < contentLength; i++)
            content_jchar[i] = (jchar) content_str[i];
        content_jchar[contentLength] = 0;
        jstring jcontent = env->NewString( content_jchar, contentLength );
        jint jdelay = static_cast<jint>(_delay);

        env->CallStaticVoidMethod( mActivityClass, jmethodID_scheduleLocalNotification, jid, jtitle, jcontent, jdelay );

        env->DeleteLocalRef( jtitle );
        env->DeleteLocalRef( jcontent );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLocalNotificationsModule::instantlyPresentLocalNotification( int _id, const WString & _title, const WString & _content )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        jint jid = static_cast<jint>(_id);
        const wchar_t * title_str = _title.c_str();
        size_t titleLength = _title.length();
        jchar* title_jchar = (jchar*)malloc((titleLength + 1)*sizeof(jchar));
        for (int i = 0; i < titleLength; i++)
            title_jchar[i] = (jchar) title_str[i];
        title_jchar[titleLength] = 0;
        jstring jtitle = env->NewString( title_jchar, titleLength );
        const wchar_t * content_str = _content.c_str();
        size_t contentLength = _content.length();
        jchar* content_jchar = (jchar*)malloc((contentLength + 1)*sizeof(jchar));
        for (int i = 0; i < contentLength; i++)
            content_jchar[i] = (jchar) content_str[i];
        content_jchar[contentLength] = 0;
        jstring jcontent = env->NewString( content_jchar, contentLength );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_instantlyPresentLocalNotification, jid, jtitle, jcontent );

        env->DeleteLocalRef( jtitle );
        env->DeleteLocalRef( jcontent );

        return true;
    }
}

#include "AndroidNativeLinkingModule.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"

#include "Environment/Android/AndroidUtils.h"

#include "pybind/pybind.hpp"

#define LINKING_JAVA_PREFIX org_Mengine_Build_Linking

static jclass mActivityClass;
static jmethodID jmethodID_openURL;
static jmethodID jmethodID_openMail;


static Mengine::AndroidNativeLinkingModule * s_androidNativeLinkingModule = nullptr;

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeLinking_1setupLinkingJNI )(JNIEnv *mEnv, jclass cls)
    {
        mActivityClass = (jclass)(mEnv->NewGlobalRef( cls ));

        jmethodID_openURL = mEnv->GetStaticMethodID( mActivityClass, "openURL", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V" );
        jmethodID_openMail = mEnv->GetStaticMethodID( mActivityClass, "openMail", "(Ljava/lang/String;)V" );
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonLinkingEventHandler
            : public Callback<LinkingEventHandler>
        {
        public:
            PythonLinkingEventHandler( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {}

        protected:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        static void androidLinkingSetEventHandler( AndroidNativeLinkingModule * _module, const pybind::object & _cb, const pybind::args & _args )
        {
            _module->setEventHandler(
                Helper::makeFactorableUnique<PythonLinkingEventHandler>( _cb, _args )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeLinkingModule::AndroidNativeLinkingModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeLinkingModule::~AndroidNativeLinkingModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLinkingModule::_initializeModule()
    {
        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::def_function_proxy_args( kernel, "androidLinkingSetEventHandler", &Detail::androidLinkingSetEventHandler, this );
        pybind::def_functor( kernel, "androidOpenUrl", this, &AndroidNativeLinkingModule::openURL );
        pybind::def_functor( kernel, "androidOpenMail", this, &AndroidNativeLinkingModule::openMail );

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        m_eventation.setMutex( mutex );

        s_androidNativeLinkingModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLinkingModule::_finalizeModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLinkingModule::addCommand( const LambdaLinkingEventHandler & _command )
    {
        m_eventation.addCommand( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLinkingModule::_update( bool _focus )
    {
        (void)_focus;

        m_eventation.invoke();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLinkingModule::setEventHandler( const LinkingEventHandlerPtr & _handler )
    {
        m_eventation.setEventHandler( _handler );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLinkingModule::openURL( const String & _url )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * url_str = _url.c_str();
        jstring jurl = env->NewStringUTF( url_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_openURL, jurl );

        env->DeleteLocalRef( jurl );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLinkingModule::openMail( const String & _email, const String & _subject, const String & _body )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const Char * email_str = _email.c_str();
        jstring jemail = env->NewStringUTF( email_str );
        const Char * subject_str = _subject.c_str();
        jstring jsubject = env->NewStringUTF( subject_str );
        const Char * body_str = _body.c_str();
        jstring jbody = env->NewStringUTF( body_str );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_openMail, jemail, jsubject, jbody );

        env->DeleteLocalRef( jemail );
        env->DeleteLocalRef( jsubject );
        env->DeleteLocalRef( jbody );

        return true;
    }
}  
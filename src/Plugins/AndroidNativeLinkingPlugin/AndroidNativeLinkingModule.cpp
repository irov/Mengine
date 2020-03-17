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

        jmethodID_openURL = mEnv->GetStaticMethodID( mActivityClass, "openURL", "(Ljava/lang/String;)V" );
        jmethodID_openMail = mEnv->GetStaticMethodID( mActivityClass, "openMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V" );
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
                Helper::makeFactorableUnique<PythonLinkingEventHandler>( MENGINE_DOCUMENT_FUNCTION, _cb, _args )
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
        pybind::def_functor( kernel, "AndroidOpenUrl", this, &AndroidNativeLinkingModule::openURL );
        pybind::def_functor( kernel, "AndroidOpenMail", this, &AndroidNativeLinkingModule::openMail );

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_eventation.initialize( mutex ) == false )
        {
            return false;
        }

        s_androidNativeLinkingModule = this;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLinkingModule::_finalizeModule()
    {
        s_androidNativeLinkingModule = nullptr;

        m_eventation.finalize();
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
    bool AndroidNativeLinkingModule::openURL( const WString & _url )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const wchar_t * url_str = _url.c_str();
        size_t urlLength = _url.length();
        jchar* url_jchar = (jchar*)malloc((urlLength + 1)*sizeof(jchar));
        for (int i = 0; i < urlLength; i++)
            url_jchar[i] = (jchar) url_str[i];
        url_jchar[urlLength] = 0;
        jstring jurl = env->NewString( url_jchar, urlLength );

        env->CallStaticVoidMethod( mActivityClass, jmethodID_openURL, jurl );

        env->DeleteLocalRef( jurl );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLinkingModule::openMail( const WString & _email, const WString & _subject, const WString & _body )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const wchar_t * email_str = _email.c_str();
        size_t emailLength = _email.length();
        jchar* email_jchar = (jchar*)malloc((emailLength + 1)*sizeof(jchar));
        for (int i = 0; i < emailLength; i++)
            email_jchar[i] = (jchar) email_str[i];
        email_jchar[emailLength] = 0;
        jstring jemail = env->NewString( email_jchar, emailLength );
        free(email_jchar);

        const wchar_t * subject_str = _subject.c_str();
        size_t subjectLength = _subject.length();
        jchar* subject_jchar = (jchar*)malloc((subjectLength + 1)*sizeof(jchar));
        for (int i = 0; i < subjectLength; i++)
            subject_jchar[i] = (jchar) subject_str[i];
        subject_jchar[subjectLength] = 0;
        jstring jsubject = env->NewString( subject_jchar, subjectLength );
        free(subject_jchar);

        const wchar_t * body_str = _body.c_str();
        size_t bodyLength = _body.length();
        jchar* body_jchar = (jchar*)malloc((bodyLength + 1)*sizeof(jchar));
        for (int i = 0; i < bodyLength; i++)
            body_jchar[i] = (jchar) body_str[i];
        body_jchar[bodyLength] = 0;
        jstring jbody = env->NewString( body_jchar, bodyLength );
        free(body_jchar);

        env->CallStaticVoidMethod( mActivityClass, jmethodID_openMail, jemail, jsubject, jbody );

        env->DeleteLocalRef( jemail );
        env->DeleteLocalRef( jsubject );
        env->DeleteLocalRef( jbody );

        return true;
    }
}
#include "AndroidNativeLinkingModule.h"

#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Callback.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"
#include "Kernel/AllocatorHelper.h"

#include "Environment/Android/AndroidUtils.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "pybind/pybind.hpp"
#endif

#define LINKING_JAVA_PREFIX org_Mengine_Build_Linking

static jclass mActivityClass;
static jmethodID jmethodID_openURL;
static jmethodID jmethodID_openMail;

static Mengine::AndroidNativeLinkingModule * s_androidNativeLinkingModule = nullptr;

extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL
        MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidNativeLinking_1setupLinkingJNI )(JNIEnv * env, jclass cls)
    {
        mActivityClass = (jclass)(env->NewGlobalRef( cls ));

        jmethodID_openURL = env->GetStaticMethodID( mActivityClass, "linkingOpenURL", "(Ljava/lang/String;)Z" );
        jmethodID_openMail = env->GetStaticMethodID( mActivityClass, "linkingOpenMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z" );
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
        class PythonLinkingEventHandler
            : public Callback<LinkingEventHandler>
        {
        public:
            PythonLinkingEventHandler( const pybind::object & _cb, const pybind::args & _args )
                : m_cb( _cb )
                , m_args( _args )
            {
            }

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
#endif
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
#ifdef MENGINE_USE_SCRIPT_SERVICE
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
                ->getKernel();

        pybind::def_function_proxy_args( kernel, "androidLinkingSetEventHandler", &Detail::androidLinkingSetEventHandler, this );
        pybind::def_functor( kernel, "AndroidOpenUrl", this, &AndroidNativeLinkingModule::openURL );
        pybind::def_functor( kernel, "AndroidOpenMail", this, &AndroidNativeLinkingModule::openMail );
#endif

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
        MENGINE_UNUSED( _focus );

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
        jchar * url_jchar = (jchar *)Helper::allocateMemory( (urlLength + 1) * sizeof( jchar ), "AndroidNativeLinkingModule" );
        for( int i = 0; i != urlLength; ++i ) {
            url_jchar[i] = (jchar) url_str[i];
        }
        url_jchar[urlLength] = 0;
        jstring jurl = env->NewString( url_jchar, urlLength );
        Helper::deallocateMemory( url_jchar, "AndroidNativeLinkingModule" );

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_openURL, jurl );

        env->DeleteLocalRef( jurl );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLinkingModule::openMail( const WString & _email, const WString & _subject, const WString & _body )
    {
        JNIEnv * env = Mengine_JNI_GetEnv();

        const wchar_t * email_str = _email.c_str();
        size_t emailLength = _email.length();
        jchar * email_jchar = (jchar *)Helper::allocateMemory( (emailLength + 1) * sizeof( jchar ), "AndroidNativeLinkingModule" );
        for( int i = 0; i != emailLength; ++i ) {
            email_jchar[i] = (jchar) email_str[i];
        }
        email_jchar[emailLength] = 0;
        jstring jemail = env->NewString( email_jchar, emailLength );
        Helper::deallocateMemory( email_jchar, "AndroidNativeLinkingModule" );

        const wchar_t * subject_str = _subject.c_str();
        size_t subjectLength = _subject.length();
        jchar * subject_jchar = (jchar *)Helper::allocateMemory( (subjectLength + 1) * sizeof( jchar ), "AndroidNativeLinkingModule" );
        for( int i = 0; i != subjectLength; ++i ) {
            subject_jchar[i] = (jchar) subject_str[i];
        }
        subject_jchar[subjectLength] = 0;
        jstring jsubject = env->NewString( subject_jchar, subjectLength );
        Helper::deallocateMemory( subject_jchar, "AndroidNativeLinkingModule" );

        const wchar_t * body_str = _body.c_str();
        size_t bodyLength = _body.length();
        jchar * body_jchar = (jchar *)Helper::allocateMemory( (bodyLength + 1) * sizeof( jchar ), "AndroidNativeLinkingModule" );
        for( int i = 0; i != bodyLength; ++i ) {
            body_jchar[i] = (jchar) body_str[i];
        }
        body_jchar[bodyLength] = 0;
        jstring jbody = env->NewString( body_jchar, bodyLength );
        Helper::deallocateMemory( body_jchar, "AndroidNativeLinkingModule" );

        jboolean jReturnValue = env->CallStaticBooleanMethod( mActivityClass, jmethodID_openMail, jemail, jsubject, jbody );

        env->DeleteLocalRef( jemail );
        env->DeleteLocalRef( jsubject );
        env->DeleteLocalRef( jbody );

        return (bool)jReturnValue;
    }
    //////////////////////////////////////////////////////////////////////////
}
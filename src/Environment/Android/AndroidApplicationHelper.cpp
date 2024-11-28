#include "AndroidApplicationHelper.h"

#include "AndroidEnv.h"
#include "AndroidHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Error.h"

#include "Config/StdArg.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        jmethodID AndroidGetApplicationMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature )
        {
            jclass jclassMengineApplication = Mengine_JNI_GetJClassMengineApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jclassMengineApplication != nullptr, "invalid get android application class for method '%s' with signature '%s'"
                , _name
                , _signature
            );

            jmethodID jmethodID_function = _jenv->GetMethodID( jclassMengineApplication, _name, _signature );

            MENGINE_ASSERTION_FATAL( jmethodID_function != nullptr, "invalid get android application method '%s' with signature '%s'"
                , _name
                , _signature
            );

            return jmethodID_function;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Helper::AndroidGetApplicationMethodID( _jenv, _name, _signature );

            jobject jobjectMengineApplication = Mengine_JNI_GetJObjectMengineApplication();

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            _jenv->CallVoidMethodV( jobjectMengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Helper::AndroidGetApplicationMethodID( _jenv, _name, _signature );

            jobject jobjectMengineApplication = Mengine_JNI_GetJObjectMengineApplication();

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = _jenv->CallObjectMethodV( jobjectMengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Helper::AndroidGetApplicationMethodID( _jenv, _name, _signature );

            jobject jobjectMengineApplication = Mengine_JNI_GetJObjectMengineApplication();

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean jresult = _jenv->CallBooleanMethodV( jobjectMengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Helper::AndroidGetApplicationMethodID( _jenv, _name, _signature );

            jobject jobjectMengineApplication = Mengine_JNI_GetJObjectMengineApplication();

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jint jresult = _jenv->CallIntMethodV( jobjectMengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Helper::AndroidGetApplicationMethodID( _jenv, _name, _signature );

            jobject jobjectMengineApplication = Mengine_JNI_GetJObjectMengineApplication();

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jlong jresult = _jenv->CallLongMethodV( jobjectMengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
#include "AndroidFragmentHelper.h"

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
        void AndroidCallVoidFragmentMethod( MengineJNIEnvThread * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... )
        {
            jmethodID jmethod_Fragment = Helper::AndroidEnvGetMethodFragment( _jenv, _fragment, _method, _signature );

            jobject jobject_Fragment = Helper::AndroidEnvGetObjectFragment( _jenv, _fragment );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            Mengine_JNI_CallVoidMethodV( _jenv, jobject_Fragment, jmethod_Fragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_Fragment );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectFragmentMethod( MengineJNIEnvThread * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... )
        {
            jmethodID jmethod_Fragment = Helper::AndroidEnvGetMethodFragment( _jenv, _fragment, _method, _signature );

            jobject jobject_Fragment = Helper::AndroidEnvGetObjectFragment( _jenv, _fragment );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = Mengine_JNI_CallObjectMethodV( _jenv, jobject_Fragment, jmethod_Fragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_Fragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanFragmentMethod( MengineJNIEnvThread * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... )
        {
            jmethodID jmethod_Fragment = Helper::AndroidEnvGetMethodFragment( _jenv, _fragment, _method, _signature );

            jobject jobject_Fragment = Helper::AndroidEnvGetObjectFragment( _jenv, _fragment );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean jresult = Mengine_JNI_CallBooleanMethodV( _jenv, jobject_Fragment, jmethod_Fragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_Fragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... )
        {
            jmethodID jmethod_Fragment = Helper::AndroidEnvGetMethodFragment( _jenv, _fragment, _method, _signature );

            jobject jobject_Fragment = Helper::AndroidEnvGetObjectFragment( _jenv, _fragment );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jint jresult = Mengine_JNI_CallIntMethodV( _jenv, jobject_Fragment, jmethod_Fragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_Fragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongFragmentMethod( MengineJNIEnvThread * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... )
        {
            jmethodID jmethod_Fragment = Helper::AndroidEnvGetMethodFragment( _jenv, _fragment, _method, _signature );

            jobject jobject_Fragment = Helper::AndroidEnvGetObjectFragment( _jenv, _fragment );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jlong jresult = Mengine_JNI_CallLongMethodV( _jenv, jobject_Fragment, jmethod_Fragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_Fragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
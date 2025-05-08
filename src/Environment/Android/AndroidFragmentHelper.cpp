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
        void AndroidCallVoidFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... )
        {
            jmethodID jmethodFragment = Helper::AndroidEnvGetMethodFragment( _jenv, _fragment, _method, _signature );

            jobject jobjectFragment = Helper::AndroidEnvGetObjectFragment( _jenv, _fragment );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            _jenv->CallVoidMethodV( jobjectFragment, jmethodFragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectFragment );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... )
        {
            jmethodID jmethodFragment = Helper::AndroidEnvGetMethodFragment( _jenv, _fragment, _method, _signature );

            jobject jobjectFragment = Helper::AndroidEnvGetObjectFragment( _jenv, _fragment );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = _jenv->CallObjectMethodV( jobjectFragment, jmethodFragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectFragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... )
        {
            jmethodID jmethodFragment = Helper::AndroidEnvGetMethodFragment( _jenv, _fragment, _method, _signature );

            jobject jobjectFragment = Helper::AndroidEnvGetObjectFragment( _jenv, _fragment );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean jresult = _jenv->CallBooleanMethodV( jobjectFragment, jmethodFragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectFragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntApplicationMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... )
        {
            jmethodID jmethodFragment = Helper::AndroidEnvGetMethodFragment( _jenv, _fragment, _method, _signature );

            jobject jobjectFragment = Helper::AndroidEnvGetObjectFragment( _jenv, _fragment );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jint jresult = _jenv->CallIntMethodV( jobjectFragment, jmethodFragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectFragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... )
        {
            jmethodID jmethodFragment = Helper::AndroidEnvGetMethodFragment( _jenv, _fragment, _method, _signature );

            jobject jobjectFragment = Helper::AndroidEnvGetObjectFragment( _jenv, _fragment );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jlong jresult = _jenv->CallLongMethodV( jobjectFragment, jmethodFragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectFragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
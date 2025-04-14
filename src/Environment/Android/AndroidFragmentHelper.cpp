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
        void AndroidCallVoidFragmentMethod( JNIEnv * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... )
        {
            jmethodID jmethodFragment = Mengine_JNI_GetMethodFragment( _jenv, _fragmentName, _methodName, _methodSignature );

            MENGINE_ASSERTION_FATAL( jmethodFragment != nullptr, "invalid get method '%s' signature '%s' fragment '%s'"
                , _methodName
                , _methodSignature
                , _fragmentName
            );

            jobject jobjectFragment = Mengine_JNI_GetObjectFragment( _jenv, _fragmentName );

            MENGINE_ASSERTION_FATAL( jobjectFragment != nullptr, "invalid get object fragment '%s'"
                , _fragmentName
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _methodSignature );

            _jenv->CallVoidMethodV( jobjectFragment, jmethodFragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectFragment );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectFragmentMethod( JNIEnv * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... )
        {
            jmethodID jmethodFragment = Mengine_JNI_GetMethodFragment( _jenv, _fragmentName, _methodName, _methodSignature );

            MENGINE_ASSERTION_FATAL( jmethodFragment != nullptr, "invalid get method '%s' signature '%s' fragment '%s'"
                , _methodName
                , _methodSignature
                , _fragmentName
            );

            jobject jobjectFragment = Mengine_JNI_GetObjectFragment( _jenv, _fragmentName );

            MENGINE_ASSERTION_FATAL( jobjectFragment != nullptr, "invalid get object fragment '%s'"
                , _fragmentName
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _methodSignature );

            jobject jresult = _jenv->CallObjectMethodV( jobjectFragment, jmethodFragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectFragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanFragmentMethod( JNIEnv * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... )
        {
            jmethodID jmethodFragment = Mengine_JNI_GetMethodFragment( _jenv, _fragmentName, _methodName, _methodSignature );

            MENGINE_ASSERTION_FATAL( jmethodFragment != nullptr, "invalid get method '%s' signature '%s' fragment '%s'"
                , _methodName
                , _methodSignature
                , _fragmentName
            );

            jobject jobjectFragment = Mengine_JNI_GetObjectFragment( _jenv, _fragmentName );

            MENGINE_ASSERTION_FATAL( jobjectFragment != nullptr, "invalid get object fragment '%s'"
                , _fragmentName
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _methodSignature );

            jboolean jresult = _jenv->CallBooleanMethodV( jobjectFragment, jmethodFragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectFragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntApplicationMethod( JNIEnv * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... )
        {
            jmethodID jmethodFragment = Mengine_JNI_GetMethodFragment( _jenv, _fragmentName, _methodName, _methodSignature );

            MENGINE_ASSERTION_FATAL( jmethodFragment != nullptr, "invalid get method '%s' signature '%s' fragment '%s'"
                , _methodName
                , _methodSignature
                , _fragmentName
            );

            jobject jobjectFragment = Mengine_JNI_GetObjectFragment( _jenv, _fragmentName );

            MENGINE_ASSERTION_FATAL( jobjectFragment != nullptr, "invalid get object fragment '%s'"
                , _fragmentName
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _methodSignature );

            jint jresult = _jenv->CallIntMethodV( jobjectFragment, jmethodFragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectFragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongFragmentMethod( JNIEnv * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... )
        {
            jmethodID jmethodFragment = Mengine_JNI_GetMethodFragment( _jenv, _fragmentName, _methodName, _methodSignature );

            MENGINE_ASSERTION_FATAL( jmethodFragment != nullptr, "invalid get method '%s' signature '%s' fragment '%s'"
                , _methodName
                , _methodSignature
                , _fragmentName
            );

            jobject jobjectFragment = Mengine_JNI_GetObjectFragment( _jenv, _fragmentName );

            MENGINE_ASSERTION_FATAL( jobjectFragment != nullptr, "invalid get object fragment '%s'"
                , _fragmentName
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _methodSignature );

            jlong jresult = _jenv->CallLongMethodV( jobjectFragment, jmethodFragment, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectFragment );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
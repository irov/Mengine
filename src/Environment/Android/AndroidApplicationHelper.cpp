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
        void AndroidCallVoidApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodApplication( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' application"
                , _name
                , _signature
            );

            jobject jobject_MengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jobject_MengineApplication != nullptr, "invalid get object application '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            Mengine_JNI_CallVoidMethodV( _jenv, jobject_MengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineApplication );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodApplication( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' application"
                , _name
                , _signature
            );

            jobject jobject_MengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jobject_MengineApplication != nullptr, "invalid get object application '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = Mengine_JNI_CallObjectMethodV( _jenv, jobject_MengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineApplication );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodApplication( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' application"
                , _name
                , _signature
            );

            jobject jobject_MengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jobject_MengineApplication != nullptr, "invalid get object application '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean jresult = Mengine_JNI_CallBooleanMethodV( _jenv, jobject_MengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineApplication );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodApplication( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' application"
                , _name
                , _signature
            );

            jobject jobject_MengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jobject_MengineApplication != nullptr, "invalid get object application '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jint jresult = Mengine_JNI_CallIntMethodV( _jenv, jobject_MengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineApplication );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodApplication( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' application"
                , _name
                , _signature
            );

            jobject jobject_MengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jobject_MengineApplication != nullptr, "invalid get object application '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jlong jresult = Mengine_JNI_CallLongMethodV( _jenv, jobject_MengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineApplication );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
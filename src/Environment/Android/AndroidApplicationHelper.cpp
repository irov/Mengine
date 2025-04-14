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
        void AndroidCallVoidApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodApplication( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' application"
                , _name
                , _signature
            );

            jobject jobjectMengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jobjectMengineApplication != nullptr, "invalid get object application '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            _jenv->CallVoidMethodV( jobjectMengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectMengineApplication );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodApplication( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' application"
                , _name
                , _signature
            );

            jobject jobjectMengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jobjectMengineApplication != nullptr, "invalid get object application '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = _jenv->CallObjectMethodV( jobjectMengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectMengineApplication );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodApplication( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' application"
                , _name
                , _signature
            );

            jobject jobjectMengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jobjectMengineApplication != nullptr, "invalid get object application '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean jresult = _jenv->CallBooleanMethodV( jobjectMengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectMengineApplication );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodApplication( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' application"
                , _name
                , _signature
            );

            jobject jobjectMengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jobjectMengineApplication != nullptr, "invalid get object application '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jint jresult = _jenv->CallIntMethodV( jobjectMengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectMengineApplication );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodApplication( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' application"
                , _name
                , _signature
            );

            jobject jobjectMengineApplication = Mengine_JNI_GetObjectApplication( _jenv );

            MENGINE_ASSERTION_FATAL( jobjectMengineApplication != nullptr, "invalid get object application '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jlong jresult = _jenv->CallLongMethodV( jobjectMengineApplication, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectMengineApplication );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
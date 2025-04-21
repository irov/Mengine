#include "AndroidActivityHelper.h"

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
        void AndroidCallVoidActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jobject jobjectMengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jobjectMengineActivity != nullptr, "invalid get object activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            _jenv->CallVoidMethodV( jobjectMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectMengineActivity );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jobject jobjectMengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jobjectMengineActivity != nullptr, "invalid get object activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = _jenv->CallObjectMethodV( jobjectMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectMengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jobject jobjectMengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jobjectMengineActivity != nullptr, "invalid get object activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean jresult = _jenv->CallBooleanMethodV( jobjectMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectMengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jobject jobjectMengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jobjectMengineActivity != nullptr, "invalid get object activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jint jresult = _jenv->CallIntMethodV( jobjectMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectMengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jobject jobjectMengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jobjectMengineActivity != nullptr, "invalid get object activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jlong jresult = _jenv->CallLongMethodV( jobjectMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jobjectMengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidActivityStaticMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jclass jclassMengineActivity = Mengine_JNI_GetClassActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jclassMengineActivity != nullptr, "invalid get class activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            _jenv->CallStaticVoidMethodV( jclassMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclassMengineActivity );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectActivityStaticMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jclass jclassMengineActivity = Mengine_JNI_GetClassActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jclassMengineActivity != nullptr, "invalid get class activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = _jenv->CallStaticObjectMethodV( jclassMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclassMengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanActivityStaticMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jclass jclassMengineActivity = Mengine_JNI_GetClassActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jclassMengineActivity != nullptr, "invalid get class activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean jresult = _jenv->CallStaticBooleanMethodV( jclassMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclassMengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntActivityStaticMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jclass jclassMengineActivity = Mengine_JNI_GetClassActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jclassMengineActivity != nullptr, "invalid get class activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jint jresult = _jenv->CallStaticIntMethodV( jclassMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclassMengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongActivityStaticMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jclass jclassMengineActivity = Mengine_JNI_GetClassActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jclassMengineActivity != nullptr, "invalid get class activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jlong jresult = _jenv->CallStaticLongMethodV( jclassMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            _jenv->DeleteLocalRef( jclassMengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
   }
}
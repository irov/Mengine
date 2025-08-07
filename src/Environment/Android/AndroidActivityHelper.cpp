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
        void AndroidCallVoidActivityMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jobject jobject_MengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jobject_MengineActivity != nullptr, "invalid get object activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            Mengine_JNI_CallVoidMethodV( _jenv, jobject_MengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineActivity );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectActivityMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jobject jobject_MengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jobject_MengineActivity != nullptr, "invalid get object activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = Mengine_JNI_CallObjectMethodV( _jenv, jobject_MengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanActivityMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jobject jobject_MengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jobject_MengineActivity != nullptr, "invalid get object activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean jresult = Mengine_JNI_CallBooleanMethodV( _jenv, jobject_MengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntActivityMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jobject jobject_MengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jobject_MengineActivity != nullptr, "invalid get object activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jint jresult = Mengine_JNI_CallIntMethodV( _jenv, jobject_MengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongActivityMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jobject jobject_MengineActivity = Mengine_JNI_GetObjectActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jobject_MengineActivity != nullptr, "invalid get object activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jlong jresult = Mengine_JNI_CallLongMethodV( _jenv, jobject_MengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_MengineActivity );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidActivityStaticMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jclass jclass_MengineActivity = Mengine_JNI_GetClassActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jclass_MengineActivity != nullptr, "invalid get class activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            Mengine_JNI_CallStaticVoidMethodV( _jenv, jclass_MengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectActivityStaticMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jclass jclass_MengineActivity = Mengine_JNI_GetClassActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jclass_MengineActivity != nullptr, "invalid get class activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = Mengine_JNI_CallStaticObjectMethodV( _jenv, jclass_MengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanActivityStaticMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jclass jclass_MengineActivity = Mengine_JNI_GetClassActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jclass_MengineActivity != nullptr, "invalid get class activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean jresult = Mengine_JNI_CallStaticBooleanMethodV( _jenv, jclass_MengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntActivityStaticMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jclass jclass_MengineActivity = Mengine_JNI_GetClassActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jclass_MengineActivity != nullptr, "invalid get class activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jint jresult = Mengine_JNI_CallStaticIntMethodV( _jenv, jclass_MengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongActivityStaticMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Mengine_JNI_GetMethodActivity( _jenv, _name, _signature );

            Helper::AndroidEnvExceptionCheck( _jenv );

            MENGINE_ASSERTION_FATAL( jmethodId != nullptr, "invalid get method '%s' signature '%s' activity"
                , _name
                , _signature
            );

            jclass jclass_MengineActivity = Mengine_JNI_GetClassActivity( _jenv );

            MENGINE_ASSERTION_FATAL( jclass_MengineActivity != nullptr, "invalid get class activity '%s'"
                , _name
            );

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jlong jresult = Mengine_JNI_CallStaticLongMethodV( _jenv, jclass_MengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::AndroidEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
   }
}
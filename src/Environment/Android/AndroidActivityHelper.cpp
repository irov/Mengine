#include "AndroidActivityHelper.h"

#include "AndroidEnv.h"
#include "AndroidHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Error.h"
#include "Kernel/StringCopy.h"

#include "Config/StdArg.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        jmethodID AndroidGetActivityMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature )
        {
            jclass jclassMengineActivity = Mengine_JNI_GetJClassMengineActivity();

            jmethodID jmethodID_function = _jenv->GetMethodID( jclassMengineActivity, _name, _signature );

            MENGINE_ASSERTION_FATAL( jmethodID_function != nullptr, "invalid get android activity method '%s' with signature '%s'"
                , _name
                , _signature
            );

            return jmethodID_function;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Helper::AndroidGetActivityMethodID( _jenv, _name, _signature );

            jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            _jenv->CallVoidMethodV( jobjectMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::jEnvExceptionCheck( _jenv );
        }
        //////////////////////////////////////////////////////////////////////////
        jobject AndroidCallObjectActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Helper::AndroidGetActivityMethodID( _jenv, _name, _signature );

            jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jobject jresult = _jenv->CallObjectMethodV( jobjectMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::jEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jboolean AndroidCallBooleanActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Helper::AndroidGetActivityMethodID( _jenv, _name, _signature );

            jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jboolean jresult = _jenv->CallBooleanMethodV( jobjectMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::jEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jint AndroidCallIntActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Helper::AndroidGetActivityMethodID( _jenv, _name, _signature );

            jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jint jresult = _jenv->CallIntMethodV( jobjectMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::jEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
        jlong AndroidCallLongActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... )
        {
            jmethodID jmethodId = Helper::AndroidGetActivityMethodID( _jenv, _name, _signature );

            jobject jobjectMengineActivity = Mengine_JNI_GetJObjectMengineActivity();

            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _signature );

            jlong jresult = _jenv->CallLongMethodV( jobjectMengineActivity, jmethodId, args );

            MENGINE_VA_LIST_END( args );

            Helper::jEnvExceptionCheck( _jenv );

            return jresult;
        }
        //////////////////////////////////////////////////////////////////////////
   }
}
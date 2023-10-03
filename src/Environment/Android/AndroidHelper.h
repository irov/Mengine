#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/String.h"
#include "Kernel/ArrayString.h"
#include "Kernel/StaticString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        StaticString<1024> getJavaClassName( JNIEnv * _jenv, jclass _jclass );
        //////////////////////////////////////////////////////////////////////////
        jobject makeJObjectBoolean( JNIEnv * _jenv, bool _value );
        jobject makeJObjectCharacter( JNIEnv * _jenv, Char _value );
        jobject makeJObjectInteger( JNIEnv * _jenv, int32_t _value );
        jobject makeJObjectLong( JNIEnv * _jenv, int64_t _value );
        jobject makeJObjectFloat( JNIEnv * _jenv, float _value );
        jobject makeJObjectDouble( JNIEnv * _jenv, double _value );
        jobject makeJObjectString( JNIEnv * _jenv, const Char * _value );
        jobject makeJObjectString( JNIEnv * _jenv, const String & _value );
        jobject makeJObjectString( JNIEnv * _jenv, const ConstString & _value );
        jobject makeJObjectArrayList( JNIEnv * _jenv, int32_t _count );
        jobject makeJObjectHashMap( JNIEnv * _jenv, int32_t _count );
        //////////////////////////////////////////////////////////////////////////
        jboolean getJavaObjectValueBoolean( JNIEnv * _jenv, jobject _jobject );
        jchar getJavaObjectValueCharacter( JNIEnv * _jenv, jobject _jobject );
        jint getJavaObjectValueInteger( JNIEnv * _jenv, jobject _jobject );
        jlong getJavaObjectValueLong( JNIEnv * _jenv, jobject _jobject );
        jfloat getJavaObjectValueFloat( JNIEnv * _jenv, jobject _jobject );
        jdouble getJavaObjectValueDouble( JNIEnv * _jenv, jobject _jobject );
        //////////////////////////////////////////////////////////////////////////
        ConstString makeConstStringFromJString( JNIEnv * _jenv, jstring _value );
        FilePath makeFilePathFromJString( JNIEnv * _jenv, jstring _value );
        String makeStringFromJString( JNIEnv * _jenv, jstring _value );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( jobject key, jobject value)> LambdaJavaMapForeach;
        void foreachJavaMap( JNIEnv * _jenv, jobject _jmap, const LambdaJavaMapForeach & _lambda );
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( jsize _index, jobject value)> LambdaJavaListForeach;
        uint32_t getJavaListSize( JNIEnv * _jenv, jobject _jlist );
        void foreachJavaList( JNIEnv * _jenv, jobject _jlist, const LambdaJavaListForeach & _lambda );
        //////////////////////////////////////////////////////////////////////////
        void jEnvExceptionCheck( JNIEnv * _jenv );
        //////////////////////////////////////////////////////////////////////////
    }
}

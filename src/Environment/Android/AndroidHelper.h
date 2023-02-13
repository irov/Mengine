#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        jobject makeJObjectBoolean( JNIEnv * _jenv, bool _value );
        jobject makeJObjectInteger( JNIEnv * _jenv, int32_t _value );
        jobject makeJObjectLong( JNIEnv * _jenv, int64_t _value );
        jobject makeJObjectFloat( JNIEnv * _jenv, float _value );
        jobject makeJObjectDouble( JNIEnv * _jenv, double _value );
        jobject makeJObjectString( JNIEnv * _jenv, const Char * _value );
        jobject makeJObjectHashMap( JNIEnv * _jenv, int32_t _count );
        //////////////////////////////////////////////////////////////////////////
        Mengine::ConstString makeConstStringFromJString( JNIEnv * _jenv, jstring _value );
        Mengine::String makeStringFromJString( JNIEnv * _jenv, jstring _value );
        //////////////////////////////////////////////////////////////////////////
        void jEnvExceptionCheck( JNIEnv * _jenv );
        //////////////////////////////////////////////////////////////////////////
    }
}

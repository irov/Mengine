#pragma once

#include "Environment/Android/AndroidEnv.h"

#include "Kernel/JSON.h"
#include "Kernel/String.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_NODISCARD jobject AndroidGetJavaJSONObjectElement( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key );
        MENGINE_NODISCARD jobject AndroidGetJavaJSONObjectObject( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key );
        MENGINE_NODISCARD jobject AndroidGetJavaJSONObjectArray( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key );
        bool AndroidGetJavaJSONObjectBoolean( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, bool _default );
        jint AndroidGetJavaJSONObjectInt( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, jint _default );
        jlong AndroidGetJavaJSONObjectLong( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, jlong _default );
        jdouble AndroidGetJavaJSONObjectDouble( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, jdouble _default );
        String AndroidGetJavaJSONObjectString( JNIEnv * _jenv, jclass _jclass, jobject _jobject, const Char * _key, const Char * _default );
        //////////////////////////////////////////////////////////////////////////
        bool AndroidMakeJSONFromJObject( JNIEnv * _jenv, jobject _jobject, jpp::object * const _json );
        //////////////////////////////////////////////////////////////////////////
    }
}

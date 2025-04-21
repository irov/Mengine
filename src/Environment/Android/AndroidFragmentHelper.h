#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidFragmentMethod( JNIEnv * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... );
        MENGINE_NODISCARD jobject AndroidCallObjectFragmentMethod( JNIEnv * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... );
        jboolean AndroidCallBooleanFragmentMethod( JNIEnv * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... );
        jint AndroidCallIntFragmentMethod( JNIEnv * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... );
        jlong AndroidCallLongFragmentMethod( JNIEnv * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... );
        //////////////////////////////////////////////////////////////////////////
    }
}

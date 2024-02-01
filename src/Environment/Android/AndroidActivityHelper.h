#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        jmethodID AndroidGetActivityMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature );
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... );
        jobject AndroidCallObjectActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... );
        jboolean AndroidCallBooleanActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... );
        jint AndroidCallIntActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... );
        jlong AndroidCallLongActivityMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... );
        //////////////////////////////////////////////////////////////////////////
    }
}

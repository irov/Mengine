#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        jmethodID AndroidGetApplicationMethodID( JNIEnv * _jenv, const Char * _name, const Char * _signature );
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... );
        jobject AndroidCallObjectApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... );
        jboolean AndroidCallBooleanApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... );
        jint AndroidCallIntApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... );
        jlong AndroidCallLongApplicationMethod( JNIEnv * _jenv, const Char * _name, const Char * _signature, ... );
        //////////////////////////////////////////////////////////////////////////
    }
}

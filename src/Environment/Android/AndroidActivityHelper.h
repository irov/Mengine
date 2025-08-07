#pragma once

#include "Environment/Android/AndroidEnv.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidActivityMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        MENGINE_NODISCARD jobject AndroidCallObjectActivityMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        jboolean AndroidCallBooleanActivityMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        jint AndroidCallIntActivityMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        jlong AndroidCallLongActivityMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidActivityStaticMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        MENGINE_NODISCARD jobject AndroidCallObjectActivityStaticMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        jboolean AndroidCallBooleanActivityStaticMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        jint AndroidCallIntActivityStaticMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        jlong AndroidCallLongActivityStaticMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        //////////////////////////////////////////////////////////////////////////
    }
}

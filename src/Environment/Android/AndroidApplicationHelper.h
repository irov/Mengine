#pragma once

#include "Environment/Android/AndroidEnv.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        MENGINE_NODISCARD jobject AndroidCallObjectApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        jboolean AndroidCallBooleanApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        jint AndroidCallIntApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        jlong AndroidCallLongApplicationMethod( MengineJNIEnvThread * _jenv, const Char * _name, const Char * _signature, ... );
        //////////////////////////////////////////////////////////////////////////
    }
}

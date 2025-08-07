#pragma once

#include "Environment/Android/AndroidEnv.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidFragmentMethod( MengineJNIEnvThread * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... );
        MENGINE_NODISCARD jobject AndroidCallObjectFragmentMethod( MengineJNIEnvThread * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... );
        jboolean AndroidCallBooleanFragmentMethod( MengineJNIEnvThread * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... );
        jint AndroidCallIntFragmentMethod( MengineJNIEnvThread * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... );
        jlong AndroidCallLongFragmentMethod( MengineJNIEnvThread * _jenv, const Char * _fragmentName, const Char * _methodName, const Char * _methodSignature, ... );
        //////////////////////////////////////////////////////////////////////////
    }
}

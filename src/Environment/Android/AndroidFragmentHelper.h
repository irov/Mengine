#pragma once

#include "Environment/Android/AndroidEnv.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... );
        MENGINE_NODISCARD jobject AndroidCallObjectFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... );
        jboolean AndroidCallBooleanFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... );
        jint AndroidCallIntFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... );
        jlong AndroidCallLongFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... );
        jfloat AndroidCallFloatFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... );
        jdouble AndroidCallDoubleFragmentMethod( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature, ... );
        //////////////////////////////////////////////////////////////////////////
    }
}

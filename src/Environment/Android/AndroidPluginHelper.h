#pragma once

#include "Environment/Android/AndroidEnv.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void AndroidCallVoidPluginMethod( JNIEnv * _jenv, const Char * _plugin, const Char * _method, const Char * _signature, ... );
        jboolean AndroidCallBooleanPluginMethod( JNIEnv * _jenv, const Char * _plugin, const Char * _method, const Char * _signature, ... );
        MENGINE_NODISCARD jobject AndroidCallObjectPluginMethod( JNIEnv * _jenv, const Char * _plugin, const Char * _method, const Char * _signature, ... );
        //////////////////////////////////////////////////////////////////////////
    }
}

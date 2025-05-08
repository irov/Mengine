#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"

namespace Mengine
{
    int Mengine_JNI_Initialize( JNIEnv * _env );
    JNIEnv * Mengine_JNI_GetEnv();
    int Mengine_JNI_SetupThread();

    jclass Mengine_JNI_GetClassApplication( JNIEnv * _jenv );
    jobject Mengine_JNI_GetObjectApplication( JNIEnv * _jenv );
    jmethodID Mengine_JNI_GetMethodApplication( JNIEnv * _jenv, const Char * _method, const Char * _signature );

    jclass Mengine_JNI_GetClassActivity( JNIEnv * _jenv );
    jobject Mengine_JNI_GetObjectActivity( JNIEnv * _jenv );
    jmethodID Mengine_JNI_GetMethodActivity( JNIEnv * _jenv, const Char * _method, const Char * _signature );

    jclass Mengine_JNI_GetClassFragment( JNIEnv * _jenv, const Char * _fragment );
    jobject Mengine_JNI_GetObjectFragment( JNIEnv * _jenv, const Char * _fragment );
    jmethodID Mengine_JNI_GetMethodFragment( JNIEnv * _jenv, const Char * _fragment, const Char * _method, const Char * _signature );

    jclass Mengine_JNI_FindClass( JNIEnv * _jenv, const Char * _class );
}
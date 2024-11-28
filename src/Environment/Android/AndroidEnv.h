#pragma once

#include "Environment/Android/AndroidIncluder.h"

namespace Mengine
{
    int Mengine_JNI_Initialize( JNIEnv *_env );
    JNIEnv * Mengine_JNI_GetEnv();
    int Mengine_JNI_SetupThread();

    jboolean Mengine_JNI_ExistMengineApplication();
    jclass Mengine_JNI_GetJClassMengineApplication( JNIEnv * _jenv );
    jobject Mengine_JNI_GetJObjectMengineApplication();

    jboolean Mengine_JNI_ExistMengineActivity();
    jclass Mengine_JNI_GetJClassMengineActivity( JNIEnv * _jenv );
    jobject Mengine_JNI_GetJObjectMengineActivity();

    jclass Mengine_JNI_FindClass( JNIEnv *_jenv, const char *_className );
}
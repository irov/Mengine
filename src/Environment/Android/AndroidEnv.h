#pragma once

#include "Environment/Android/AndroidIncluder.h"

extern "C" 
{
    JNIEnv * Mengine_JNI_GetEnv( void );
    jboolean Mengine_JNI_ExistMengineApplication( void );
    jclass Mengine_JNI_GetJClassMengineApplication( void );
    jobject Mengine_JNI_GetJObjectMengineApplication( void );
    jboolean Mengine_JNI_ExistMengineActivity( void );
    jclass Mengine_JNI_GetJClassMengineActivity( void );
    jobject Mengine_JNI_GetJObjectMengineActivity( void );
    jclass Mengine_JNI_FindClass( JNIEnv * _jenv, const char * _className );
}
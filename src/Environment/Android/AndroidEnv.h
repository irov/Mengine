#pragma once

#include "Environment/Android/AndroidIncluder.h"

#define MENGINE_JAVA_CONCAT2(prefix, class, function) Java_##prefix##_##class##_## function
#define MENGINE_JAVA_FUNCTION_INTERFACE(prefix, class, function) MENGINE_JAVA_CONCAT2(prefix, class, function)

#define MENGINE_JAVA_PREFIX org_Mengine_Base
#define MENGINE_LOG_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineLog, function)
#define MENGINE_APPLICATION_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineApplication, function)
#define MENGINE_ACTIVITY_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineActivity, function)
#define MENGINE_SURFACE_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineSurface, function)

extern "C" 
{
    JNIEnv * Mengine_JNI_GetEnv( void );
}
#pragma once

#include "Environment/Android/AndroidIncluder.h"

#define MENGINE_JAVA_CONCAT2(prefix, class, function) Java_##prefix##_##class##_## function
#define MENGINE_JAVA_FUNCTION_INTERFACE(prefix, class, function) MENGINE_JAVA_CONCAT2(prefix, class, function)

#define MENGINE_JAVA_PREFIX org_Mengine_Base
#define MENGINE_LOG_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineLog, function)
#define MENGINE_SEMAPHORE_LISTENER_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineSemaphoreListener, function)
#define MENGINE_ANALYTICS_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineAnalytics, function)
#define MENGINE_ANALYTICSEVENTBUILDER_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineAnalyticsEventBuilder, function)
#define MENGINE_APPLICATION_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineApplication, function)
#define MENGINE_ACTIVITY_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineActivity, function)
#define MENGINE_SURFACE_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineSurface, function)

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
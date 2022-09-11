#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"

#define MENGINE_JAVA_CONCAT2(prefix, class, function) Java_##prefix##_##class##_## function
#define MENGINE_JAVA_FUNCTION_INTERFACE(prefix, class, function) MENGINE_JAVA_CONCAT2(prefix, class, function)

#define MENGINE_JAVA_PREFIX org_Mengine_Base
#define MENGINE_LOG_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineLog, function)
#define MENGINE_ACTIVITY_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineActivity, function)
#define MENGINE_SURFACE_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineSurface, function)

extern "C" 
{
    JNIEnv * Mengine_JNI_GetEnv( void );
}

namespace Mengine
{
    namespace Helper
    {
        int32_t AndroidOpenAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const Char * _path );
        int32_t AndroidAvailableAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId );
        int32_t AndroidReadAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer );
        int64_t AndroidSkipAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId, int64_t _offset );
        void AndroidResetAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId );
        void AndroidCloseAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId );

        bool AndroidOpenMail( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const Char * _email, const Char * _subject, const Char * _body );
        bool AndroidOpenUrlInDefaultBrowser( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const Char * _url );
    }
}
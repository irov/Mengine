#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"

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
    }
}
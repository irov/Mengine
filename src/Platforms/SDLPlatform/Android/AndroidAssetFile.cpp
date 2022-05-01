#include "AndroidAssetFile.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidOpenAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const char * _path )
    {
        static jmethodID jmethodID_openAssetFile = jenv->GetMethodID( jclass_activity, "openAssetFile", "(Ljava/lang/String;)I" );

        if( jmethodID_openAssetFile == nullptr )
        {
            LOGGER_ERROR("invalid get android method 'openAssetFile'");

            return 0;
        }

        jstring jpath = jenv->NewStringUTF( _path );

        jint jfileId = jenv->CallIntMethod( jobject_activity, jmethodID_openAssetFile, jpath );

        jenv->DeleteLocalRef( jpath );

        return jfileId;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidAvailableAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId )
    {
        static jmethodID jmethodID_availableAssetFile = jenv->GetMethodID( jclass_activity, "availableAssetFile", "(I)I" );

        if( jmethodID_availableAssetFile == nullptr )
        {
            LOGGER_ERROR("invalid get android method 'availableAssetFile'");

            return 0;
        }

        jint javailable = jenv->CallIntMethod( jobject_activity, jmethodID_availableAssetFile, _fileId );

        return javailable;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t AndroidReadAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer )
    {
        static jmethodID jmethodID_readAssetFile = jenv->GetMethodID( jclass_activity, "readAssetFile", "(III)[B" );

        if( jmethodID_readAssetFile == nullptr )
        {
            LOGGER_ERROR("invalid get android method 'readAssetFile'");

            return 0;
        }

        jbyteArray jbuffer = (jbyteArray)jenv->CallObjectMethod( jobject_activity, jmethodID_readAssetFile, _fileId, _offset, _size );

        int32_t len = jenv->GetArrayLength( jbuffer );

        jenv->GetByteArrayRegion( jbuffer, 0, len, reinterpret_cast<jbyte *>(_buffer) );

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidSkipAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId, int64_t _offset )
    {
        static jmethodID jmethodID_skipAssetFile = jenv->GetMethodID( jclass_activity, "skipAssetFile", "(IJ)J" );

        if( jmethodID_skipAssetFile == nullptr )
        {
            LOGGER_ERROR("invalid get android method 'skipAssetFile'");

            return 0;
        }

        jlong javailable = jenv->CallIntMethod( jobject_activity, jmethodID_skipAssetFile, _fileId, _offset );

        return javailable;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidResetAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId )
    {
        static jmethodID jmethodID_resetAssetFile = jenv->GetMethodID( jclass_activity, "resetAssetFile", "(I)V" );

        if( jmethodID_resetAssetFile == nullptr )
        {
            LOGGER_ERROR("invalid get android method 'resetAssetFile'");

            return;
        }

        jenv->CallVoidMethod( jobject_activity, jmethodID_resetAssetFile, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidCloseAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId )
    {
        static jmethodID jmethodID_closeAssetFile = jenv->GetMethodID( jclass_activity, "closeAssetFile", "(I)V" );

        if( jmethodID_closeAssetFile == nullptr )
        {
            LOGGER_ERROR("invalid get android method 'closeAssetFile'");

            return;
        }

        jenv->CallVoidMethod( jobject_activity, jmethodID_closeAssetFile, _fileId );
    }
    //////////////////////////////////////////////////////////////////////////
}
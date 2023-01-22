#include "AndroidAssetFile.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        int32_t AndroidOpenAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const Char * _path )
        {
            static jmethodID jmethodID_openAssetFile = jenv->GetMethodID( jclass_activity, "openAssetFile", "(Ljava/lang/String;)I" );

            MENGINE_ASSERTION( jmethodID_openAssetFile != nullptr, "invalid get android method 'openAssetFile'" );

            jstring jpath = jenv->NewStringUTF( _path );

            jint jfileId = jenv->CallIntMethod( jobject_activity, jmethodID_openAssetFile, jpath );

            jenv->DeleteLocalRef( jpath );

            return jfileId;
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t AndroidAvailableAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId )
        {
            static jmethodID jmethodID_availableAssetFile = jenv->GetMethodID( jclass_activity, "availableAssetFile", "(I)I" );

            MENGINE_ASSERTION( jmethodID_availableAssetFile != nullptr, "invalid get android method 'availableAssetFile'" );

            jint javailable = jenv->CallIntMethod( jobject_activity, jmethodID_availableAssetFile, _fileId );

            return javailable;
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t AndroidReadAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer )
        {
            static jmethodID jmethodID_readAssetFile = jenv->GetMethodID( jclass_activity, "readAssetFile", "(III)[B" );

            MENGINE_ASSERTION( jmethodID_readAssetFile != nullptr, "invalid get android method 'readAssetFile'" );

            jbyteArray jbuffer = (jbyteArray)jenv->CallObjectMethod( jobject_activity, jmethodID_readAssetFile, _fileId, _offset, _size );

            int32_t len = jenv->GetArrayLength( jbuffer );

            jenv->GetByteArrayRegion( jbuffer, 0, len, reinterpret_cast<jbyte *>(_buffer) );

            jenv->DeleteLocalRef( jbuffer );

            return len;
        }
        //////////////////////////////////////////////////////////////////////////
        int64_t AndroidSkipAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId, int64_t _offset )
        {
            static jmethodID jmethodID_skipAssetFile = jenv->GetMethodID( jclass_activity, "skipAssetFile", "(IJ)J" );

            MENGINE_ASSERTION( jmethodID_skipAssetFile != nullptr, "invalid get android method 'skipAssetFile'" );

            jlong javailable = jenv->CallIntMethod( jobject_activity, jmethodID_skipAssetFile, _fileId, _offset );

            return javailable;
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidResetAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId )
        {
            static jmethodID jmethodID_resetAssetFile = jenv->GetMethodID( jclass_activity, "resetAssetFile", "(I)V" );

            MENGINE_ASSERTION( jmethodID_resetAssetFile != nullptr, "invalid get android method 'resetAssetFile'" );

            jenv->CallVoidMethod( jobject_activity, jmethodID_resetAssetFile, _fileId );
        }
        //////////////////////////////////////////////////////////////////////////
        void AndroidCloseAssetFile( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, int32_t _fileId )
        {
            static jmethodID jmethodID_closeAssetFile = jenv->GetMethodID( jclass_activity, "closeAssetFile", "(I)V" );

            MENGINE_ASSERTION( jmethodID_closeAssetFile != nullptr, "invalid get android method 'closeAssetFile'" );

            jenv->CallVoidMethod( jobject_activity, jmethodID_closeAssetFile, _fileId );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
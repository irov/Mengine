#include "AndroidUtils.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"

#include <pthread.h>

extern "C" 
{
    //////////////////////////////////////////////////////////////////////////
    static pthread_key_t mThreadKey;
    static JavaVM * mJavaVM;
    //////////////////////////////////////////////////////////////////////////
    static void Mengine_JNI_ThreadDestroyed( void *value )
    {
        /* The thread is being destroyed, detach it from the Java VM and set the mThreadKey value to NULL as required */
        JNIEnv *env = (JNIEnv *)value;
        if( env != NULL ) {
            mJavaVM->DetachCurrentThread();
            pthread_setspecific( mThreadKey, NULL );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEnv * Mengine_JNI_GetEnv( void )
    {
        /* From http://developer.android.com/guide/practices/jni.html
        * All threads are Linux threads, scheduled by the kernel.
        * They're usually started from managed code (using Thread.start), but they can also be created elsewhere and then
        * attached to the JavaVM. For example, a thread started with pthread_create can be attached with the
        * JNI AttachCurrentThread or AttachCurrentThreadAsDaemon functions. Until a thread is attached, it has no JNIEnv,
        * and cannot make JNI calls.
        * Attaching a natively-created thread causes a java.lang.Thread object to be constructed and added to the "main"
        * ThreadGroup, making it visible to the debugger. Calling AttachCurrentThread on an already-attached thread
        * is a no-op.
        * Note: You can call this function any number of times for the same thread, there's no harm in it
        */

        JNIEnv *env;
        int get_status = mJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
        if (get_status == JNI_OK) {
            return env;
        } else if (get_status == JNI_EDETACHED ) {
            int attach_status = mJavaVM->AttachCurrentThread(&env, NULL);
            if (attach_status < 0) {
                return 0;
            }

            /* From http://developer.android.com/guide/practices/jni.html
            * Threads attached through JNI must call DetachCurrentThread before they exit. If coding this directly is awkward,
            * in Android 2.0 (Eclair) and higher you can use pthread_key_create to define a destructor function that will be
            * called before the thread exits, and call DetachCurrentThread from there. (Use that key with pthread_setspecific
            * to store the JNIEnv in thread-local-storage; that way it'll be passed into your destructor as the argument.)
            * Note: The destructor is not called unless the stored value is != NULL
            * Note: You can call this function any number of times for the same thread, there's no harm in it
            *       (except for some lost CPU cycles)
            */
            pthread_setspecific(mThreadKey, (void *) env);

            return env;
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    static int Mengine_JNI_SetupThread( void )
    {
        Mengine_JNI_GetEnv();

        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM *vm, void *reserved ) {
        mJavaVM = vm;

        JNIEnv *env;
        if( mJavaVM->GetEnv( (void **)&env, JNI_VERSION_1_6 ) != JNI_OK ) {
            return JNI_ERR;
        }
        /*
        * Create mThreadKey so we can keep track of the JNIEnv assigned to each thread
        * Refer to http://developer.android.com/guide/practices/design/jni.html for the rationale behind this
        */
        if( pthread_key_create( &mThreadKey, Mengine_JNI_ThreadDestroyed ) != 0 ) {
            return JNI_ERR;
        }

        Mengine_JNI_SetupThread();

        return JNI_VERSION_1_6;
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
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
        bool AndroidOpenMail( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const Char * _email, const Char * _subject, const Char * _body )
        {
            static jmethodID jmethodID_linkingOpenMail = jenv->GetMethodID( jclass_activity, "linkingOpenMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z" );

            MENGINE_ASSERTION( jmethodID_linkingOpenMail != nullptr, "invalid get android method 'linkingOpenMail'" );

            jstring jemail = jenv->NewStringUTF( _email );
            jstring jsubject = jenv->NewStringUTF( _subject );
            jstring jbody = jenv->NewStringUTF( _body );

            jboolean jReturnValue = jenv->CallBooleanMethod( jobject_activity, jmethodID_linkingOpenMail, jemail, jsubject, jbody );

            jenv->DeleteLocalRef( jemail );
            jenv->DeleteLocalRef( jsubject );
            jenv->DeleteLocalRef( jbody );

            return jReturnValue;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AndroidOpenUrlInDefaultBrowser( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const Char * _url )
        {
            static jmethodID jmethodID_linkingOpenURL = jenv->GetMethodID( jclass_activity, "linkingOpenURL", "(Ljava/lang/String;)Z" );

            MENGINE_ASSERTION( jmethodID_linkingOpenURL != nullptr, "invalid get android method 'linkingOpenURL'" );

            jstring jurl = jenv->NewStringUTF( _url );

            jboolean jReturnValue = jenv->CallBooleanMethod( jobject_activity, jmethodID_linkingOpenURL, jurl );

            jenv->DeleteLocalRef( jurl );

            return jReturnValue;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
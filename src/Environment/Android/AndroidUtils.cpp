#include "AndroidUtils.h"

#include <pthread.h>

extern "C" 
{
    //////////////////////////////////////////////////////////////////////////
    static pthread_key_t mThreadKey;
    static JavaVM *mJavaVM;
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
    JNIEnv *Mengine_JNI_GetEnv( void )
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
        int status = mJavaVM->AttachCurrentThread( &env, NULL );
        if( status < 0 ) {
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
        pthread_setspecific( mThreadKey, (void *)env );

        return env;
    }
    //////////////////////////////////////////////////////////////////////////
    static int Mengine_JNI_SetupThread( void )
    {
        Mengine_JNI_GetEnv();

        return 1;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM *vm, void *reserved ) {
        JNIEnv *env;
        mJavaVM = vm;
        if( mJavaVM->GetEnv( (void **)&env, JNI_VERSION_1_4 ) != JNI_OK ) {
            return -1;
        }
        /*
        * Create mThreadKey so we can keep track of the JNIEnv assigned to each thread
        * Refer to http://developer.android.com/guide/practices/design/jni.html for the rationale behind this
        */
        if( pthread_key_create( &mThreadKey, Mengine_JNI_ThreadDestroyed ) != 0 ) {
            return -1;
        }

        Mengine_JNI_SetupThread();

        return JNI_VERSION_1_4;
    }
    //////////////////////////////////////////////////////////////////////////
}
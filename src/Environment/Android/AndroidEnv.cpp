#include "AndroidEnv.h"

#include <pthread.h>

extern "C" 
{
    //////////////////////////////////////////////////////////////////////////
    static pthread_key_t g_androidEnvThreadKey;
    static JavaVM * g_androidEnvJavaVM;
    //////////////////////////////////////////////////////////////////////////
    JNIEnv * Mengine_JNI_GetEnv( void )
    {
        JNIEnv *env;
        int get_status = g_androidEnvJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
        if (get_status == JNI_OK) {
            return env;
        } else if (get_status == JNI_EDETACHED ) {
            int attach_status = g_androidEnvJavaVM->AttachCurrentThread(&env, nullptr);
            if (attach_status < 0) {
                return nullptr;
            }

            pthread_setspecific(g_androidEnvThreadKey, (void *) env);

            return env;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    static void Mengine_JNI_ThreadDestroyed( void *value )
    {
        /* The thread is being destroyed, detach it from the Java VM and set the mThreadKey value to NULL as required */
        JNIEnv *env = (JNIEnv *)value;
        if( env != NULL ) {
            g_androidEnvJavaVM->DetachCurrentThread();
            pthread_setspecific( g_androidEnvThreadKey, NULL );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM *vm, void *reserved ) {
        g_androidEnvJavaVM = vm;

        JNIEnv *env;
        if(g_androidEnvJavaVM->GetEnv((void **)&env, JNI_VERSION_1_6 ) != JNI_OK ) {
            return JNI_ERR;
        }

        return JNI_VERSION_1_6;
    }
    //////////////////////////////////////////////////////////////////////////
}
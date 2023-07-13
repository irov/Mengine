#include "AndroidEnv.h"

#include <pthread.h>

extern "C" 
{
    //////////////////////////////////////////////////////////////////////////
    static jclass g_jclass_MengineApplication;
    static jobject g_jobject_MengineApplication;
    //////////////////////////////////////////////////////////////////////////
    static jclass g_jclass_MengineActivity;
    static jobject g_jobject_MengineActivity;
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_APPLICATION_JAVA_INTERFACE( AndroidEnv_1setMengineAndroidApplicationJNI )(JNIEnv * env, jclass cls, jobject obj)
    {
        g_jclass_MengineApplication = (jclass)env->NewGlobalRef( cls );
        g_jobject_MengineApplication = (jclass)env->NewGlobalRef( obj );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_APPLICATION_JAVA_INTERFACE( AndroidEnv_1removeMengineAndroidApplicationJNI )(JNIEnv * env, jclass cls)
    {
        env->DeleteGlobalRef( g_jclass_MengineApplication );
        g_jclass_MengineApplication = nullptr;

        env->DeleteGlobalRef( g_jobject_MengineApplication );
        g_jobject_MengineApplication = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnv_1setMengineAndroidActivityJNI )(JNIEnv * env, jclass cls, jobject obj)
    {
        g_jclass_MengineActivity = (jclass)env->NewGlobalRef( cls );
        g_jobject_MengineActivity = (jclass)env->NewGlobalRef( obj );
    }
    //////////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_ACTIVITY_JAVA_INTERFACE( AndroidEnv_1removeMengineAndroidActivityJNI )(JNIEnv * env, jclass cls)
    {
        env->DeleteGlobalRef( g_jclass_MengineActivity );
        g_jclass_MengineActivity = nullptr;

        env->DeleteGlobalRef( g_jobject_MengineActivity );
        g_jobject_MengineActivity = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetJClassMengineApplication( void )
    {
        return g_jclass_MengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetJObjectMengineApplication( void )
    {
        return g_jobject_MengineApplication;
    }
    //////////////////////////////////////////////////////////////////////////
    jclass Mengine_JNI_GetJClassMengineActivity( void )
    {
        return g_jclass_MengineActivity;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject Mengine_JNI_GetJObjectMengineActivity( void )
    {
        return g_jobject_MengineActivity;
    }
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
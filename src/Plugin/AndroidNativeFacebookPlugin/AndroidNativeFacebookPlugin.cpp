#include "AndroidNativeFacebookPlugin.h"

#include "Core/ModuleFactory.h"

#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <vector>
#include <pthread.h>

extern "C" {
#define FACEBOOK_JAVA_PREFIX                             org_Mengine_Build_Facebook
#define MENGINE_JAVA_PREFIX                              org_Mengine_Build
#define CONCAT1(prefix, class, function)                 CONCAT2(prefix, class, function)
#define CONCAT2(prefix, class, function)                 Java_ ## prefix ## _ ## class ## _ ## function
#define MENGINE_JAVA_INTERFACE(function)                 CONCAT1(MENGINE_JAVA_PREFIX, MengineActivity, function)
#define FACEBOOK_JAVA_INTERFACE(function)                CONCAT1(FACEBOOK_JAVA_PREFIX, FacebookInteractionLayer, function)

JNIEXPORT void JNICALL
MENGINE_JAVA_INTERFACE(setupFacebookJNI)(JNIEnv *mEnv, jclass cls);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginSuccess)(JNIEnv *env, jclass cls,
                                        jstring accessToken_);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginCancel)(JNIEnv *env, jclass cls);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginError)(JNIEnv *env, jclass cls,
                                      jstring exception_);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onUserFetchSuccess)(JNIEnv *env, jclass cls,
                                            jstring object_, jstring response_);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareSuccess)(JNIEnv *env, jclass cls,
                                        jstring postId_);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareCancel)(JNIEnv *env, jclass cls);

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareError)(JNIEnv *env, jclass cls,
                                      jstring exception_);
}

static pthread_key_t mThreadKey;
static JavaVM *mJavaVM;

static jclass mActivityClass;
static jmethodID mperformLogin;
static jmethodID mgetUser;
static jmethodID mshareLink;

static FacebookLoginCallback *currentFacebookLoginCallback;
static FacebookUserCallback *currentFacebookUserCallback;
static FacebookShareCallback *currentFacebookShareCallback;

static void FB_JNI_ThreadDestroyed(void *value) {
    /* The thread is being destroyed, detach it from the Java VM and set the mThreadKey value to NULL as required */
    JNIEnv *env = (JNIEnv *) value;
    if (env != NULL) {
        mJavaVM->DetachCurrentThread();
        pthread_setspecific(mThreadKey, NULL);
    }
}

JNIEnv *FB_JNI_GetEnv(void) {
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
    int status = mJavaVM->AttachCurrentThread(&env, NULL);
    if (status < 0) {
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

int FB_JNI_SetupThread(void) {
    FB_JNI_GetEnv();
    return 1;
}

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    mJavaVM = vm;
    if (mJavaVM->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    /*
     * Create mThreadKey so we can keep track of the JNIEnv assigned to each thread
     * Refer to http://developer.android.com/guide/practices/design/jni.html for the rationale behind this
     */
    if (pthread_key_create(&mThreadKey, FB_JNI_ThreadDestroyed) != 0) {
        return -1;
    }
    FB_JNI_SetupThread();

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL
MENGINE_JAVA_INTERFACE(setupFacebookJNI)(JNIEnv *mEnv, jclass cls) {
    mActivityClass = (jclass) (mEnv->NewGlobalRef(cls));

    mperformLogin = mEnv->GetStaticMethodID(mActivityClass,
                                            "performLogin", "([Ljava/lang/String;)V");
    mgetUser = mEnv->GetStaticMethodID(mActivityClass,
                                       "getUser", "()V");
    mshareLink = mEnv->GetStaticMethodID(mActivityClass,
                                         "shareLink", "(Ljava/lang/String;)V");
}


JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginSuccess)(JNIEnv *env, jclass cls,
                                        jstring accessToken_) {
    char *accessToken = (char *) env->GetStringUTFChars(accessToken_, 0);

    if (currentFacebookLoginCallback != NULL) {
        currentFacebookLoginCallback->onLoginSuccess(accessToken);
    }

    env->ReleaseStringUTFChars(accessToken_, accessToken);
}

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginCancel)(JNIEnv *env, jclass cls) {
    if (currentFacebookLoginCallback != NULL) {
        currentFacebookLoginCallback->onLoginCancel();
    }
}

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginError)(JNIEnv *env, jclass cls,
                                      jstring exception_) {
    char *exception = (char *) env->GetStringUTFChars(exception_, 0);
    if (currentFacebookLoginCallback != NULL) {
        currentFacebookLoginCallback->onLoginError(exception);
    }
    env->ReleaseStringUTFChars(exception_, exception);
}


JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onUserFetchSuccess)(JNIEnv *env, jclass cls,
                                            jstring object_, jstring response_) {
    char *object = (char *) env->GetStringUTFChars(object_, 0);
    char *response = (char *) env->GetStringUTFChars(response_, 0);

    if (currentFacebookUserCallback != NULL) {
        currentFacebookUserCallback->onUserFetchSuccess(object, response);
    }
    env->ReleaseStringUTFChars(object_, object);
    env->ReleaseStringUTFChars(response_, response);
}


JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareSuccess)(JNIEnv *env, jclass cls,
                                        jstring postId_) {
    char *postId = (char *) env->GetStringUTFChars(postId_, 0);

    if (currentFacebookShareCallback != NULL) {
        currentFacebookShareCallback->onShareSuccess(postId);
    }
    env->ReleaseStringUTFChars(postId_, postId);
}

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareCancel)(JNIEnv *env, jclass cls) {
    if (currentFacebookShareCallback != NULL) {
        currentFacebookShareCallback->onShareCancel();
    }
}

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareError)(JNIEnv *env, jclass cls,
                                      jstring exception_) {
    char *exception = (char *) env->GetStringUTFChars(exception_, 0);

    if (currentFacebookShareCallback != NULL) {
        currentFacebookShareCallback->onShareError(exception);
    }
    env->ReleaseStringUTFChars(exception_, exception);
}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY(AndroidNativeFacebook, Mengine::AndroidNativeFacebookPlugin)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods to be called from Python
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Mengine {
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeFacebookPlugin::AndroidNativeFacebookPlugin() {
    }

    //////////////////////////////////////////////////////////////////////////
    AndroidNativeFacebookPlugin::~AndroidNativeFacebookPlugin() {
    }

    static void
    performLogin(FacebookLoginCallback *facebookLoginCallback,
                 std::vector<char *> *readPermissions) {
        currentFacebookLoginCallback = facebookLoginCallback;
        JNIEnv *mEnv = FB_JNI_GetEnv();
        if (readPermissions != NULL) {
            jstring stringPermissions[readPermissions->size()];
            for (size_t i = 0; i < readPermissions->size(); i++) {
                stringPermissions[i] = (jstring) (mEnv->NewStringUTF(
                        (*readPermissions)[i]));
            }
            mEnv->CallStaticVoidMethod(mActivityClass, mperformLogin, stringPermissions);
        } else {
            mEnv->CallStaticVoidMethod(mActivityClass, mperformLogin, NULL);
        }
    }

    static void getUser(FacebookUserCallback *facebookUserCallback) {
        currentFacebookUserCallback = facebookUserCallback;
        JNIEnv *mEnv = FB_JNI_GetEnv();
        mEnv->CallStaticVoidMethod(mActivityClass, mgetUser);
    }

    static void shareLink(char *link, FacebookShareCallback *facebookShareCallback) {
        currentFacebookShareCallback = facebookShareCallback;
        JNIEnv *mEnv = FB_JNI_GetEnv();
        jstring stringLink = (jstring) (mEnv->NewStringUTF(link));
        mEnv->CallStaticVoidMethod(mActivityClass, mshareLink, stringLink);
    }
}

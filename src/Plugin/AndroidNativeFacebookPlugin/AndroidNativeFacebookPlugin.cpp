//
// Created by Alex Korzh on 6/26/18.
//

#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <vector>
#include "FacebookLoginCallback.h"
#include "FacebookShareCallback.h"
#include "FacebookUserCallback.h"
#include "AndroidNativeFacebookPlugin.h"


#define MENGINE_JAVA_PREFIX                             org_Mengine_Build
#define CONCAT1(prefix, class, function)                CONCAT2(prefix, class, function)
#define CONCAT2(prefix, class, function)                Java_ ## prefix ## _ ## class ## _ ## function
#define MENGINE_JAVA_INTERFACE(function)                CONCAT1(MENGINE_JAVA_PREFIX, MengineActivity, function)

#define FACEBOOK_JAVA_PREFIX                             org_Mengine_Build_Facebook
#define CONCAT1(prefix, class, function)                 CONCAT2(prefix, class, function)
#define CONCAT2(prefix, class, function)                 Java_ ## prefix ## _ ## class ## _ ## function
#define FACEBOOK_JAVA_INTERFACE(function)                CONCAT1(FACEBOOK_JAVA_PREFIX, FacebookInteractionLayer, function)

static jclass mActivityClass;
static jmethodID mperformLogin;
static jmethodID mgetUser;
static jmethodId mshareLink;

static FacebookLoginCallback currentFacebookLoginCallback;
static FacebookUserCallback currentFacebookUserCallback;
static FacebookShareCallback currentFacebookShareCallback;


JNIEXPORT void JNICALL
MENGINE_JAVA_INTERFACE(setupFacebookJNI)(JNIEnv* mEnv, jclass cls);
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginSuccess)(JNIEnv *env, jobject instance,
        jobject loginResult);
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginCancel)(JNIEnv *env, jobject instance);
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginError)(JNIEnv *env, jobject instance,
        jstring exception_);
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onUserFetchSuccess)(JNIEnv *env, jobject instance,
        jstring object_, jstring response_);
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareSuccess)(JNIEnv *env, jobject instance,
        jstring postId_);
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareCancel)(JNIEnv *env, jobject instance);
JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareError)(JNIEnv *env, jobject instance,
        jstring exception_);


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;
    mJavaVM = vm;
    if ((*mJavaVM)->GetEnv(mJavaVM, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    /*
     * Create mThreadKey so we can keep track of the JNIEnv assigned to each thread
     * Refer to http://developer.android.com/guide/practices/design/jni.html for the rationale behind this
     */
    if (pthread_key_create(&mThreadKey, Android_JNI_ThreadDestroyed) != 0) {
        return -1;
    }
    Android_JNI_SetupThread();

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL
MENGINE_JAVA_INTERFACE(setupFacebookJNI)(JNIEnv* mEnv, jclass cls) {
    mActivityClass = (jclass)((*mEnv)->NewGlobalRef(mEnv, cls));
    mperformLogin = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass,
            "performLogin", "([Ljava/lang/String;)");
    mgetUser = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass,
            "getUser", "()");
    mshareLink = (*mEnv)->GetStaticMethodID(mEnv, mActivityClass,
            "shareLink", "(Ljava/lang/String;)");
}

JNIEnv* Android_JNI_GetEnv(void)
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
    int status = (*mJavaVM)->AttachCurrentThread(mJavaVM, &env, NULL);
    if(status < 0) {
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
    pthread_setspecific(mThreadKey, (void*) env);

    return env;
}

int Android_JNI_SetupThread(void)
{
    Android_JNI_GetEnv();
    return 1;
}


JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginSuccess)(JNIEnv *env, jobject instance,
        jobject loginResult) {
//currentFacebookLoginCallback.onLoginSuccess();
}

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginCancel)(JNIEnv *env,
jobject instance) {
currentFacebookLoginCallback.onLoginCancel();
}

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onLoginError)(JNIEnv *env, jobject instance,
        jstring exception_) {
const char *exception = (*env)->GetStringUTFChars(env, exception_, 0);

currentFacebookLoginCallback.onLoginError(exception);

(*env)->ReleaseStringUTFChars(env, exception_, exception);
}


JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onUserFetchSuccess)(JNIEnv *env, jobject instance,
        jstring object_, jstring response_) {
const char *object = (*env)->GetStringUTFChars(env, object_, 0);
const char *response = (*env)->GetStringUTFChars(env, response_, 0);

currentFacebookUserCallback.onUserFetchSuccess(object, response);

(*env)->ReleaseStringUTFChars(env, object_, object);
(*env)->ReleaseStringUTFChars(env, response_, response);
}


JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareSuccess)(JNIEnv *env, jobject instance,
        jstring postId_) {
const char *postId = (*env)->GetStringUTFChars(env, postId_, 0);

currentFacebookShareCallback.onShareSuccess(postId);

(*env)->ReleaseStringUTFChars(env, postId_, postId);
}

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareCancel)(JNIEnv *env, jobject instance) {
currentFacebookShareCallback.onShareCancel();
}

JNIEXPORT void JNICALL
FACEBOOK_JAVA_INTERFACE(onShareError)(JNIEnv *env, jobject instance,
        jstring exception_) {
const char *exception = (*env)->GetStringUTFChars(env, exception_, 0);

currentFacebookShareCallback.onShareError(exception);

(*env)->ReleaseStringUTFChars(env, exception_, exception);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods to be called from Python
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void performLogin(FacebookLoginCallback* facebookLoginCallback, std::vector<char*>* readPermissions) {
    currentFacebookLoginCallback = facebookLoginCallback;
    JNIEnv *mEnv = Android_JNI_GetEnv();
    if(readPermissions != NULL) {
        jstring stringPermissions[readPermissions->size()];
        for (size_t i = 0; i < readPermissions->size(); i++) {
            jstring stringPermissions[i] = (jstring)((*env)->NewStringUTF(env, (*readPermissions)[i]));
        }
        (*env)->CallStaticVoidMethod(env, mActivityClass, mperformLogin, stringPermissions);
    } else {
        (*env)->CallStaticVoidMethod(env, mActivityClass, mperformLogin, NULL);
    }
}

static void getUser(FacebookUserCallback* facebookUserCallback) {
    currentFacebookUserCallback = facebookUserCallback;
    JNIEnv *mEnv = Android_JNI_GetEnv();
    // TODO
    (*env)->CallStaticVoidMethod(env, mActivityClass, mgetUser);
}

static void shareLink(char* link, FacebookShareCallback* facebookShareCallback) {
    currentFacebookShareCallback = facebookShareCallback;
    JNIEnv *mEnv = Android_JNI_GetEnv();
    jstring stringLink = (jstring)((*env)->NewStringUTF(env, link));
    (*env)->CallStaticVoidMethod(env, mActivityClass, mshareLink, stringLink);
}
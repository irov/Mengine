#include "AndroidOpenUrlInDefaultBrowser.h"

namespace Mengine
{
    bool AndroidOpenUrlInDefaultBrowser( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const char * _url )
    {
        static jmethodID jmethodID_openURL = jenv->GetMethodID( jclass_activity, "linkingOpenURL", "(Ljava/lang/String;)Z" );

        jstring jurl = jenv->NewStringUTF( _url );

        jboolean jReturnValue = jenv->CallBooleanMethod( jobject_activity, jmethodID_openURL, jurl );

        jenv->DeleteLocalRef( jurl );

        return jReturnValue;
    }
}
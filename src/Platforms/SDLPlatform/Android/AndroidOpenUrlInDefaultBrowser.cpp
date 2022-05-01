#include "AndroidOpenUrlInDefaultBrowser.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    bool AndroidOpenUrlInDefaultBrowser( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const char * _url )
    {
        static jmethodID jmethodID_linkingOpenURL = jenv->GetMethodID( jclass_activity, "linkingOpenURL", "(Ljava/lang/String;)Z" );

        if( jmethodID_linkingOpenURL == nullptr )
        {
            LOGGER_ERROR("invalid get android method 'linkingOpenURL'");

            return false;
        }

        jstring jurl = jenv->NewStringUTF( _url );

        jboolean jReturnValue = jenv->CallBooleanMethod( jobject_activity, jmethodID_linkingOpenURL, jurl );

        jenv->DeleteLocalRef( jurl );

        return jReturnValue;
    }
}
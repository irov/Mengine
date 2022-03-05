#include "AndroidOpenMail.h"

namespace Mengine
{
    bool AndroidOpenMail( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const char * _email, const char * _subject, const char * _body )
    {
        MENGINE_UNUSED( jobject_activity );

        static jmethodID jmethodID_openURL = jenv->GetMethodID( jclass_activity, "linkingOpenMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z" );

        jstring jemail = jenv->NewStringUTF( _email );
        jstring jsubject = jenv->NewStringUTF( _subject );
        jstring jbody = jenv->NewStringUTF( _body );

        jboolean jReturnValue = jenv->CallBooleanMethod( jclass_activity, jmethodID_openURL, jemail, jsubject, jbody );

        jenv->DeleteLocalRef( jemail );
        jenv->DeleteLocalRef( jsubject );
        jenv->DeleteLocalRef( jbody );

        return jReturnValue;
    }
}
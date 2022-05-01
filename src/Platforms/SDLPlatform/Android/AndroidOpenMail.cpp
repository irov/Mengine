#include "AndroidOpenMail.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    bool AndroidOpenMail( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const char * _email, const char * _subject, const char * _body )
    {
        MENGINE_UNUSED( jobject_activity );

        static jmethodID jmethodID_linkingOpenMail = jenv->GetMethodID( jclass_activity, "linkingOpenMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z" );

        if( jmethodID_linkingOpenMail == nullptr )
        {
            LOGGER_ERROR("invalid get android method 'linkingOpenMail'");

            return false;
        }

        jstring jemail = jenv->NewStringUTF( _email );
        jstring jsubject = jenv->NewStringUTF( _subject );
        jstring jbody = jenv->NewStringUTF( _body );

        jboolean jReturnValue = jenv->CallBooleanMethod( jobject_activity, jmethodID_linkingOpenMail, jemail, jsubject, jbody );

        jenv->DeleteLocalRef( jemail );
        jenv->DeleteLocalRef( jsubject );
        jenv->DeleteLocalRef( jbody );

        return jReturnValue;
    }
}
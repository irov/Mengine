#include "AndroidUtils.h"
#include "AndroidHelper.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool AndroidOpenMail( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const Char * _email, const Char * _subject, const Char * _body )
        {
            jmethodID jmethodID_linkingOpenMail = jenv->GetMethodID( jclass_activity, "linkingOpenMail", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z" );

            MENGINE_ASSERTION( jmethodID_linkingOpenMail != nullptr, "invalid get android method 'linkingOpenMail'" );

            jstring jemail = jenv->NewStringUTF( _email );
            jstring jsubject = jenv->NewStringUTF( _subject );
            jstring jbody = jenv->NewStringUTF( _body );

            jboolean jReturnValue = jenv->CallBooleanMethod( jobject_activity, jmethodID_linkingOpenMail, jemail, jsubject, jbody );

            Helper::jEnvExceptionCheck( jenv );

            jenv->DeleteLocalRef( jemail );
            jenv->DeleteLocalRef( jsubject );
            jenv->DeleteLocalRef( jbody );

            return jReturnValue;
        }
        //////////////////////////////////////////////////////////////////////////
        bool AndroidOpenUrlInDefaultBrowser( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const Char * _url )
        {
            jmethodID jmethodID_linkingOpenURL = jenv->GetMethodID( jclass_activity, "linkingOpenURL", "(Ljava/lang/String;)Z" );

            MENGINE_ASSERTION( jmethodID_linkingOpenURL != nullptr, "invalid get android method 'linkingOpenURL'" );

            jstring jurl = jenv->NewStringUTF( _url );

            jboolean jReturnValue = jenv->CallBooleanMethod( jobject_activity, jmethodID_linkingOpenURL, jurl );

            Helper::jEnvExceptionCheck( jenv );

            jenv->DeleteLocalRef( jurl );

            return jReturnValue;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        bool AndroidOpenMail( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const Char * _email, const Char * _subject, const Char * _body );
        bool AndroidOpenUrlInDefaultBrowser( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const Char * _url );
    }
}
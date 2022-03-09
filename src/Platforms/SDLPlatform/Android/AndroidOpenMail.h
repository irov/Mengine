#pragma once

#include "Config/Config.h"

#include "Environment/Android/AndroidIncluder.h"

namespace Mengine
{
    bool AndroidOpenMail( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const char * _email, const char * _subject, const char * _body );
}


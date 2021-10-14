#pragma once

#include "Config/Config.h"

#include "Environment/Android/AndroidIncluder.h"

namespace Mengine
{
    bool AndroidOpenUrlInDefaultBrowser( JNIEnv * jenv, jclass jclass_activity, jobject jobject_activity, const char * _url );
}


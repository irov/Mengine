#include "AndroidHttpRequestGetMessageThreadTask.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestGetMessageThreadTask::AndroidHttpRequestGetMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestGetMessageThreadTask::~AndroidHttpRequestGetMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequestGetMessageThreadTask::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidHttpRequestGetMessageThreadTask::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestGetMessage", "(Lorg/Mengine/Base/MengineHttpRequestParam;)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
        );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

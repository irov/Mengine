#include "AndroidHttpRequestDeleteMessageThreadTask.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestDeleteMessageThreadTask::AndroidHttpRequestDeleteMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestDeleteMessageThreadTask::~AndroidHttpRequestDeleteMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequestDeleteMessageThreadTask::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidHttpRequestDeleteMessageThreadTask::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestDeleteMessage", "(Lorg/Mengine/Base/MengineHttpRequestParam;)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
        );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

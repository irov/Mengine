#include "AppleHttpRequestGetMessageThreadTask.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestGetMessageThreadTask::AppleHttpRequestGetMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestGetMessageThreadTask::~AppleHttpRequestGetMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHttpRequestGetMessageThreadTask::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AppleHttpRequestGetMessageThreadTask::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        jobject jresponse = Helper::AppleCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestGetMessage", "(Lorg/Mengine/Base/MengineHttpRequestParam;)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
        );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

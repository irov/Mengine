#include "AppleHttpRequestDeleteMessageThreadTask.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestDeleteMessageThreadTask::AppleHttpRequestDeleteMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestDeleteMessageThreadTask::~AppleHttpRequestDeleteMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHttpRequestDeleteMessageThreadTask::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AppleHttpRequestDeleteMessageThreadTask::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        jobject jresponse = Helper::AppleCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestDeleteMessage", "(Lorg/Mengine/Base/MengineHttpRequestParam;)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
        );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

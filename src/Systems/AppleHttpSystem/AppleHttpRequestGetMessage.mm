#include "AppleHttpRequestGetMessage.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestGetMessage::AppleHttpRequestGetMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestGetMessage::~AppleHttpRequestGetMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHttpRequestGetMessage::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AppleHttpRequestGetMessage::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        jobject jresponse = Helper::AppleCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestGetMessage", "(Lorg/Mengine/Base/MengineHttpRequestParam;)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
        );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

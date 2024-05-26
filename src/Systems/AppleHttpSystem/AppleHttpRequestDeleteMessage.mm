#include "AppleHttpRequestDeleteMessage.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestDeleteMessage::AppleHttpRequestDeleteMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestDeleteMessage::~AppleHttpRequestDeleteMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHttpRequestDeleteMessage::initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AppleHttpRequestDeleteMessage::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        jobject jresponse = Helper::AppleCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestDeleteMessage", "(Lorg/Mengine/Base/MengineHttpRequestParam;)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
        );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

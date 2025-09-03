#include "AndroidHttpRequestDeleteMessage.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestDeleteMessage::AndroidHttpRequestDeleteMessage()
        : AndroidHttpRequest( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestDeleteMessage::~AndroidHttpRequestDeleteMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidHttpRequestDeleteMessage::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestDeleteMessage", "(Lorg/Mengine/Base/MengineParamHttpRequest;)Lorg/Mengine/Base/MengineParamHttpResponse;"
            , _jrequest
        );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

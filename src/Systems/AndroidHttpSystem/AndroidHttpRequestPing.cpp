#include "AndroidHttpRequestPing.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestPing::AndroidHttpRequestPing()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestPing::~AndroidHttpRequestPing()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidHttpRequestPing::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestPing", "(Lorg/Mengine/Base/MengineHttpRequestParam;)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
        );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

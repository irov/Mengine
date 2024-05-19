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

        //MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_CUSTOMREQUEST, "GET") );

        //this->setupWriteResponse( _curl );

        //bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        //if( OPTION_curltrace == true )
        //{
        //    LOGGER_HTTP_STATISTIC( "[HTTP] get message url '%s'"
        //        , m_url.c_str()
        //    );
        //}
    }
    //////////////////////////////////////////////////////////////////////////
}

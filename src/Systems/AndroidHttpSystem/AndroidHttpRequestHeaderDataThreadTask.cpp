#include "AndroidHttpRequestHeaderDataThreadTask.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestHeaderDataThreadTask::AndroidHttpRequestHeaderDataThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestHeaderDataThreadTask::~AndroidHttpRequestHeaderDataThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequestHeaderDataThreadTask::initialize( const Data & _data )
    {
        m_data = _data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidHttpRequestHeaderDataThreadTask::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        const Data::value_type * data_buffer = m_data.data();
        Data::size_type data_size = m_data.size();

        jbyteArray jdata = _jenv->NewByteArray( data_size );

        _jenv->SetByteArrayRegion( jdata, 0, data_size, (const jbyte *)data_buffer );

        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestHeaderData", "(Lorg/Mengine/Base/MengineHttpRequestParam;[B)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
            , jdata
        );

        _jenv->DeleteLocalRef( jdata );

        return jresponse;

        //MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_CUSTOMREQUEST, "POST") );

        //MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_POSTFIELDS, data_buffer) );
        //MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_POSTFIELDSIZE, (long)data_size) );

        //bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        //if( OPTION_curltrace == true )
        //{
        //    Stringstream ss;

        //    for( const String & header : m_headers )
        //    {
        //        ss << header;
        //        ss << std::endl;
        //    }

        //    String header_str = ss.str();

        //    LOGGER_HTTP_STATISTIC( "[HTTP] header data url '%s' header:\n %s"
        //        , m_url.c_str()
        //        , header_str.c_str()
        //    );
        //}
    }
    //////////////////////////////////////////////////////////////////////////
}

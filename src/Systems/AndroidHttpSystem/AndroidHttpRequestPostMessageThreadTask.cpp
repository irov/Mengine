#include "AndroidHttpRequestPostMessageThreadTask.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestPostMessageThreadTask::AndroidHttpRequestPostMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestPostMessageThreadTask::~AndroidHttpRequestPostMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequestPostMessageThreadTask::initialize( const HttpRequestPostProperties & _properties )
    {
        m_properties = _properties;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidHttpRequestPostMessageThreadTask::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        HttpRequestPostProperties::size_type properties_size = m_properties.size();

        jobject jproperties = Helper::AndroidMakeJObjectHashMap( _jenv, properties_size );

        for( const HttpRequestPostProperty & property : m_properties )
        {
            jobject jkey = Helper::AndroidMakeJObjectString( _jenv, property.key );
            jobject jvalue = Helper::AndroidMakeJObjectString( _jenv, property.value );

            Helper::AndroidPutJObjectMap( _jenv, jproperties, jkey, jvalue );

            _jenv->DeleteLocalRef( jkey );
            _jenv->DeleteLocalRef( jvalue );
        }

        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestPostMessage", "(Lorg/Mengine/Base/MengineHttpRequestParam;Ljava/util/Map;)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
            , jproperties
        );

        _jenv->DeleteLocalRef( jproperties );

        return jresponse;

        //MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_CUSTOMREQUEST, "POST") );

        //struct curl_httppost * lastptr = nullptr;
        //struct curl_httppost * formpost = nullptr;

        //for( auto && [key, value] : m_params )
        //{
        //    /* Fill in the  field */
        //    CURLFORMcode code_formadd = curl_formadd( &formpost,
        //        &lastptr,
        //        CURLFORM_COPYNAME, key.c_str(),
        //        CURLFORM_COPYCONTENTS, value.c_str(),
        //        CURLFORM_END );

        //    if( code_formadd != CURL_FORMADD_OK )
        //    {
        //        LOGGER_HTTP_ERROR( "[HTTP] url '%s' param '%s' value '%s' get formadd error [%u]"
        //            , m_url.c_str()
        //            , key.c_str()
        //            , value.c_str()
        //            , code_formadd
        //        );
        //    }
        //}

        //MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_HTTPPOST, formpost) );

        //m_curl_formpost = formpost;

        ///* send all data to this function  */
        //this->setupWriteResponse( _curl );

        //bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        //if( OPTION_curltrace == true )
        //{
        //    Stringstream ss;

        //    for( auto && [key, value] : m_params )
        //    {
        //        ss << key.c_str() << ": " << value;
        //        ss << std::endl;
        //    }

        //    String params_str = ss.str();

        //    LOGGER_HTTP_STATISTIC( "[HTTP] post message url '%s' params:\n %s"
        //        , m_url.c_str()
        //        , params_str.c_str()
        //    );
        //}
    }
    //////////////////////////////////////////////////////////////////////////
}

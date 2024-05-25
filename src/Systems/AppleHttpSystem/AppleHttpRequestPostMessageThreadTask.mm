#include "AppleHttpRequestPostMessageThreadTask.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestPostMessageThreadTask::AppleHttpRequestPostMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestPostMessageThreadTask::~AppleHttpRequestPostMessageThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHttpRequestPostMessageThreadTask::initialize( const HttpRequestPostProperties & _properties )
    {
        m_properties = _properties;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AppleHttpRequestPostMessageThreadTask::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        HttpRequestPostProperties::size_type properties_size = m_properties.size();

        jobject jproperties = Helper::AppleMakeJObjectHashMap( _jenv, properties_size );

        for( const HttpRequestPostProperty & property : m_properties )
        {
            jobject jkey = Helper::AppleMakeJObjectString( _jenv, property.key );
            jobject jvalue = Helper::AppleMakeJObjectString( _jenv, property.value );

            Helper::ApplePutJObjectMap( _jenv, jproperties, jkey, jvalue );

            _jenv->DeleteLocalRef( jkey );
            _jenv->DeleteLocalRef( jvalue );
        }

        jobject jresponse = Helper::AppleCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestPostMessage", "(Lorg/Mengine/Base/MengineHttpRequestParam;Ljava/util/Map;)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
            , jproperties
        );

        _jenv->DeleteLocalRef( jproperties );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

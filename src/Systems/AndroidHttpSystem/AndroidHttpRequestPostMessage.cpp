#include "AndroidHttpRequestPostMessage.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestPostMessage::AndroidHttpRequestPostMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestPostMessage::~AndroidHttpRequestPostMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestPostMessage::setPostProperties(const HttpRequestPostProperties & _properties )
    {
        m_properties = _properties;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpRequestPostProperties & AndroidHttpRequestPostMessage::getPostProperties() const
    {
        return m_properties;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidHttpRequestPostMessage::_onHttp(JNIEnv * _jenv, jobject _jrequest )
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
    }
    //////////////////////////////////////////////////////////////////////////
}

#include "AndroidHttpRequestPostMessage.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestPostMessage::AndroidHttpRequestPostMessage()
        : AndroidHttpRequest( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestPostMessage::~AndroidHttpRequestPostMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestPostMessage::setPostProperties( const HttpRequestPostProperties & _properties )
    {
        m_properties = _properties;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpRequestPostProperties & AndroidHttpRequestPostMessage::getPostProperties() const
    {
        return m_properties;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidHttpRequestPostMessage::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        HttpRequestPostProperties::size_type properties_size = m_properties.size();

        jobject jproperties = Helper::AndroidMakeJObjectHashMap( _jenv, properties_size );

        for( const HttpRequestPostProperty & property : m_properties )
        {
            jobject jobject_key = Helper::AndroidMakeJObjectString( _jenv, property.key );
            jobject jobject_value = Helper::AndroidMakeJObjectString( _jenv, property.value );

            Helper::AndroidPutJObjectMap( _jenv, jproperties, jobject_key, jobject_value );

            Mengine_JNI_DeleteLocalRef( _jenv, jobject_key );
            Mengine_JNI_DeleteLocalRef( _jenv, jobject_value );
        }

        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestPostMessage", "(Lorg/Mengine/Base/MengineParamHttpRequest;Ljava/util/Map;)Lorg/Mengine/Base/MengineParamHttpResponse;"
            , _jrequest
            , jproperties
        );

        Mengine_JNI_DeleteLocalRef( _jenv, jproperties );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

#include "AndroidHttpRequestHeaderData.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestHeaderData::AndroidHttpRequestHeaderData()
        : AndroidHttpRequest( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestHeaderData::~AndroidHttpRequestHeaderData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestHeaderData::setData( const Data & _data )
    {
        m_data = _data;
    }
    //////////////////////////////////////////////////////////////////////////
    const Data & AndroidHttpRequestHeaderData::getData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidHttpRequestHeaderData::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        const Data::value_type * data_buffer = m_data.data();
        Data::size_type data_size = m_data.size();

        jbyteArray jobject_data = Mengine_JNI_NewByteArray( _jenv, data_size );

        Mengine_JNI_SetByteArrayRegion( _jenv, jobject_data, 0, data_size, reinterpret_cast<const jbyte *>(data_buffer) );

        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestHeaderData", "(Lorg/Mengine/Base/MengineParamHttpRequest;[B)Lorg/Mengine/Base/MengineParamHttpResponse;"
            , _jrequest
            , jobject_data
        );

        Mengine_JNI_DeleteLocalRef( _jenv, jobject_data );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

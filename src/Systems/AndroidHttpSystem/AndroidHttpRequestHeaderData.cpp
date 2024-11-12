#include "AndroidHttpRequestHeaderData.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestHeaderData::AndroidHttpRequestHeaderData()
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

        jbyteArray jdata = _jenv->NewByteArray( data_size );

        _jenv->SetByteArrayRegion( jdata, 0, data_size, (const jbyte *)data_buffer );

        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestHeaderData", "(Lorg/Mengine/Base/MengineHttpRequestParam;[B)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
            , jdata
        );

        _jenv->DeleteLocalRef( jdata );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
}

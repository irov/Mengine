#include "ConstStringHolderJString.h"

#include "Kernel/HashHelper.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderJString::ConstStringHolderJString()
        : m_jenv( nullptr )
        , m_value( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderJString::~ConstStringHolderJString()
    {
        if( m_value != nullptr )
        {
            const Char * data = this->data();
            m_jenv->ReleaseStringUTFChars( m_value, data );
            m_jenv->DeleteLocalRef( m_value );
            m_value = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderJString::setJString( JNIEnv * _jenv, jstring _value )
    {
        if( m_value != nullptr )
		{
            const Char * data = this->data();
            m_jenv->ReleaseStringUTFChars( m_value, data );
            m_jenv->DeleteLocalRef( m_value );
			m_value = nullptr;
		}

        m_jenv = _jenv;
        m_value = _value;

        if( m_value != nullptr )
        {
            m_value = (jstring)m_jenv->NewLocalRef( (jobject)m_value );

            const char * data = _jenv->GetStringUTFChars( m_value, nullptr );
            size_t size = MENGINE_STRLEN( data );
            int64_t hash = Helper::makeHash( data, size );

            this->setup( data, (ConstStringHolder::size_type)size, (ConstStringHolder::hash_type)hash );
        }
        else
        {
            this->setup( "", 0, 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

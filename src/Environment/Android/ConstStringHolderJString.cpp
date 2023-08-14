#include "ConstStringHolderJString.h"

#include "Kernel/HashHelper.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderJString::ConstStringHolderJString()
        : m_value( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderJString::~ConstStringHolderJString()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderJString::setJString( JNIEnv * _jenv, jstring _value )
    {
        m_value = (jstring)_jenv->NewGlobalRef( (jobject)_value );

        const char * data = _jenv->GetStringUTFChars( m_value, nullptr );
        size_t size = MENGINE_STRLEN( data );
        int64_t hash = Helper::makeHash( data, size );

        this->setup( data, (ConstStringHolder::size_type)size, (ConstStringHolder::hash_type)hash );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderJString::removeJString( JNIEnv * _jenv )
    {
        const Char * data = this->data();
        _jenv->ReleaseStringUTFChars( m_value, data );
        _jenv->DeleteGlobalRef( m_value );
        m_value = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}

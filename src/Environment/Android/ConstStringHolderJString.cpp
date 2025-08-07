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
    void ConstStringHolderJString::setJString( MengineJNIEnvThread * _jenv, jstring _value )
    {
        m_value = (jstring)Mengine_JNI_NewGlobalRef( _jenv, (jobject)_value );

        const char * data = Mengine_JNI_GetStringUTFChars( _jenv, m_value, nullptr );
        jsize size = Mengine_JNI_GetStringLength( _jenv, m_value );

        int64_t hash = Helper::makeHash( data, size );

        this->setup( data, (ConstStringHolder::size_type)size, (ConstStringHolder::hash_type)hash );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderJString::removeJString( MengineJNIEnvThread * _jenv )
    {
        const Char * data = this->data();
        Mengine_JNI_ReleaseStringUTFChars( _jenv, m_value, data );
        Mengine_JNI_DeleteGlobalRef( _jenv, m_value );
        m_value = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}

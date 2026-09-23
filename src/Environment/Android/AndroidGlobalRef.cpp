#include "AndroidGlobalRef.h"

#include "AndroidEnv.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidGlobalRef::AndroidGlobalRef( JNIEnv * _jenv, jobject _value )
        : m_value( Mengine_JNI_NewGlobalRef( _jenv, _value ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidGlobalRef::~AndroidGlobalRef()
    {
        if( m_value == nullptr )
        {
            return;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Mengine_JNI_DeleteGlobalRef( jenv, m_value );
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidGlobalRef::getValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        AndroidGlobalRefPtr makeAndroidGlobalRef( JNIEnv * _jenv, jobject _value, const DocumentInterfacePtr & _doc )
        {
            AndroidGlobalRefPtr globalRef = Helper::makeFactorableUnique<AndroidGlobalRef>( _doc, _jenv, _value );

            if( _value != nullptr )
            {
                jobject value_gref = globalRef->getValue();

                if( value_gref == nullptr )
                {
                    return nullptr;
                }
            }

            return globalRef;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
